#include "RecordArray.h"
#include "SpatialObjectTypes.h"
#include "ByteBufferOverflowException.h"

using namespace geophile;

void RecordArray::add(Z z, const SpatialObject* spatial_object)
{
    GEOPHILE_ASSERT(_n < _capacity);
    _records[_n++].set(z, copySpatialObject(spatial_object));
}

int32_t RecordArray::remove(Z z, int64_t soid)
{
    int32_t removed = false;
    SpatialObjectKey key(z, soid);
    int32_t remove_position = position(key, true, true);
    if (remove_position >= 0 && 
        remove_position < _n) {
        Record& remove_record = _records[remove_position];
        if (key.compare(_records[remove_position].key()) == 0) {
            delete remove_record.spatialObject();
            memmove(&_records[remove_position],
                    &_records[remove_position + 1],
                    (_n - remove_position - 1) * sizeof(Record));
            _n--;
            removed = true;
        }
    }
    return removed;
}

void RecordArray::freeze()
{
    qsort(_records, _n, sizeof(Record), recordCompare);
}

Cursor* RecordArray::cursor()
{
    return new RecordArrayCursor(*this);
}

RecordArray::~RecordArray()
{
    for (int i = 0; i < _n; i++) {
        delete _records[i].spatialObject();
    }
    delete [] _records;
    delete [] _buffer;
}

SpatialObject* RecordArray::copySpatialObject(const SpatialObject* spatial_object)
{
    serialize(spatial_object);
    return deserialize();
}

RecordArray::RecordArray(const SpatialObjectTypes* spatial_object_types, uint32_t capacity)
    : OrderedIndex(spatial_object_types),
      _capacity(capacity),
      _n(0),
      _records(new Record[capacity]),
      _buffer_size(INITIAL_BUFFER_SIZE),
      _buffer(new byte[INITIAL_BUFFER_SIZE])
{}

int32_t RecordArray::position(const SpatialObjectKey& key,
                              int32_t forward_move, 
                              int32_t include_key) const
{
    int32_t position;
    int32_t lo = 0;
    int32_t hi = _n - 1;
    int32_t mid;
    int32_t found = false;
    const SpatialObjectKey* mid_key;
    while (lo <= hi) {
        mid = (lo + hi) / 2;
        mid_key = &_records[mid].key();
        int32_t c = mid_key->compare(key);
        if (c < 0) {
            lo = mid + 1;
        } else if (c > 0) {
            hi = mid - 1;
        } else {
            // key found at mid
            found = true;
            break;
        }
    }
    // Adjust position based on success of binary search and parameters
    if (found) {
        // found implies mid must have been initialized
        if (include_key) {
            position = mid;
        } else if (forward_move) {
            position = mid + 1;
        } else {
            position = mid - 1;
        }
    } else {
        // key not found
        if (forward_move) {
            position = lo;
        } else {
            position = lo - 1;
        }
    }
    return position;
}

uint32_t RecordArray::nRecords() const
{
    return _n;
}

Record RecordArray::at(int32_t position) const
{
    GEOPHILE_ASSERT(position >= 0);
    GEOPHILE_ASSERT(position < _n);
    return _records[position];
}

void RecordArray::growBuffer()
{
    uint32_t new_buffer_size = _buffer_size * 2;
    byte* new_buffer = new byte[new_buffer_size];
    memcpy(new_buffer, _buffer, _buffer_size);
    delete [] _buffer;
    _buffer = new_buffer;
    _buffer_size = new_buffer_size;
}

void RecordArray::serialize(const SpatialObject* spatial_object)
{
    int32_t serialized = false;
    do {
        ByteBuffer byte_buffer(_buffer, _buffer_size);
        try {
            byte_buffer.putInt32(spatial_object->typeId());
            spatial_object->writeTo(byte_buffer);
            byte_buffer.flip();
            serialized = true;
        } catch (ByteBufferOverflowException e) {
            _buffer_size = 0; // No need to copy current contents when growing
            growBuffer();
        }
    } while (!serialized);
}

SpatialObject* RecordArray::deserialize()
{
    ByteBuffer byte_buffer(_buffer, _buffer_size);
    int32_t type_id = byte_buffer.getInt32();
    SpatialObject* spatial_object = newSpatialObject(type_id);
    spatial_object->readFrom(byte_buffer);
    return spatial_object;
}

int32_t RecordArray::recordCompare(const void* x, const void* y)
{
    const Record* r = (const Record*) x;
    const Record* s = (const Record*) y;
    return r->key().compare(s->key());
}

Record RecordArrayCursor::next()
{
    return neighbor(true);
}

Record RecordArrayCursor::previous()
{
    return neighbor(false);
}

void RecordArrayCursor::goTo(const SpatialObjectKey& key)
{
    _start_at = key;
    state(NEVER_USED);
}

RecordArrayCursor::RecordArrayCursor(RecordArray& record_array)
    : _record_array(record_array),
      _position(0),
      _start_at(),
      _forward(true)
{}

Record RecordArrayCursor::neighbor(int32_t forward_move)
{
    switch (state()) {
        case NEVER_USED:
            startIteration(forward_move, true);
            break;
        case IN_USE:
            if ((_forward && !forward_move) || (!_forward && forward_move)) {
                startIteration(forward_move, false);
            }
            break;
        case DONE:
            GEOPHILE_ASSERT(current().eof());
            return current();
    }
    if (_position >= 0 && _position < _record_array.nRecords()) {
        Record record = _record_array.at(_position);
        _position += forward_move ? 1 : -1;
        // Replace record.spatialObject() by a copy for use (and deletion) by caller.
        current(record.key().z(), _record_array.copySpatialObject(record.spatialObject()));
        state(IN_USE);
        _start_at = record.key();
    } else {
        close();
    }
    _forward = forward_move;
    return current();
}

void RecordArrayCursor::startIteration(int32_t forward_move, int32_t include_start_key)
{
    _position = _record_array.position(_start_at, forward_move, include_start_key);
}
