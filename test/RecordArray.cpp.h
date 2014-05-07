#include "RecordArray.h"
#include "SpatialObjectTypes.h"
#include "ByteBuffer.h"
#include "ByteBufferOverflowException.h"

using namespace geophile;

template <typename SOR>
void RecordArray<SOR>::add(Z z, const SOR& sor)
{
    GEOPHILE_ASSERT(_n < _capacity);
    _records[_n++].set(z, sor);
}

template <typename SOR>
SOR RecordArray<SOR>::remove(Z z, int64_t soid)
{
    SOR removed;
    SpatialObjectReference::setNull(removed);
    SpatialObjectKey key(z, soid);
    int32_t remove_position = position(key, 
                                       /* forward_move */ true, 
                                       /* include_key */ true);
    if (remove_position >= 0 && remove_position < _n) {
        Record<SOR>& remove_record = _records[remove_position];
        if (key.compare(_records[remove_position].key()) == 0) {
            removed = remove_record.spatialObjectReference();
            // Delete the spatial object if we've removed the last key
            // associated with it. THIS DOESN'T WORK IF:
            // - SOR - const SpatialObject*
            // - For a given soids, there are multiple copies of the "same" spatial object.
            // This implementation assumes there is one spatial object associated with all
            // of the soid's z-values, and that that spatial object needs to be deleted
            // once the last (z, soid) has been removed.
            int32_t after_position = remove_position + 1;
            int32_t before_position = remove_position - 1;
            if (after_position < _n && _records[after_position].key().soid() == soid) {
                // Present after removed record
            } else if (before_position >= 0 && _records[before_position].key().soid() == soid) {
                // Present before removed record
            } else {
                // Not present before or after, so delete the spatial object.
            }
            // Remove the record
            memmove(&_records[remove_position],
                    &_records[remove_position + 1],
                    (_n - remove_position - 1) * sizeof(Record<SOR>));
            _n--;
        }
    }
    return removed;
}

template <typename SOR>
void RecordArray<SOR>::freeze()
{
    qsort(_records, _n, sizeof(Record<SOR>), recordCompare);
}

template <typename SOR>
Cursor<SOR>* RecordArray<SOR>::cursor()
{
    return new RecordArrayCursor<SOR>(*this);
}

template <typename SOR>
RecordArray<SOR>::~RecordArray()
{
    for (int i = 0; i < _n; i++) {
        SpatialObjectReference::deleteSpatialObject(_records[i].spatialObjectReference());
    }
    delete [] _records;
    delete [] _buffer;
}

template <typename SOR>
RecordArray<SOR>::RecordArray(const SpatialObjectTypes* spatial_object_types, uint32_t capacity)
    : OrderedIndex<SOR>(spatial_object_types),
      _capacity(capacity),
      _n(0),
      _records(new Record<SOR>[capacity]),
      _buffer_size(INITIAL_BUFFER_SIZE),
      _buffer(new byte[INITIAL_BUFFER_SIZE])
{}

template <typename SOR>
int32_t RecordArray<SOR>::position(const SpatialObjectKey& key,
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

template <typename SOR>
uint32_t RecordArray<SOR>::nRecords() const
{
    return _n;
}

template <typename SOR>
Record<SOR> RecordArray<SOR>::at(int32_t position) const
{
    GEOPHILE_ASSERT(position >= 0);
    GEOPHILE_ASSERT(position < _n);
    return _records[position];
}

template <typename SOR>
void RecordArray<SOR>::growBuffer()
{
    uint32_t new_buffer_size = _buffer_size * 2;
    byte* new_buffer = new byte[new_buffer_size];
    memcpy(new_buffer, _buffer, _buffer_size);
    delete [] _buffer;
    _buffer = new_buffer;
    _buffer_size = new_buffer_size;
}

template <typename SOR>
void RecordArray<SOR>::serialize(const SpatialObject* spatial_object)
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

template <typename SOR>
SpatialObject* RecordArray<SOR>::deserialize()
{
    ByteBuffer byte_buffer(_buffer, _buffer_size);
    int32_t type_id = byte_buffer.getInt32();
    SpatialObject* spatial_object = this->newSpatialObject(type_id);
    spatial_object->readFrom(byte_buffer);
    return spatial_object;
}

template <typename SOR>
int32_t RecordArray<SOR>::recordCompare(const void* x, const void* y)
{
    const Record<SOR>* r = (const Record<SOR>*) x;
    const Record<SOR>* s = (const Record<SOR>*) y;
    return r->key().compare(s->key());
}

template <typename SOR>
Record<SOR> RecordArrayCursor<SOR>::next()
{
    return neighbor(true);
}

template <typename SOR>
Record<SOR> RecordArrayCursor<SOR>::previous()
{
    return neighbor(false);
}

template <typename SOR>
void RecordArrayCursor<SOR>::goTo(const SpatialObjectKey& key)
{
    _start_at = key;
    this->state(NEVER_USED);
}

template <typename SOR>
RecordArrayCursor<SOR>::RecordArrayCursor(RecordArray<SOR>& record_array)
    : _record_array(record_array),
      _position(0),
      _start_at(),
      _forward(true)
{}

template <typename SOR>
Record<SOR> RecordArrayCursor<SOR>::neighbor(int32_t forward_move)
{
    switch (this->state()) {
        case NEVER_USED:
            startIteration(forward_move, true);
            break;
        case IN_USE:
            if ((_forward && !forward_move) || (!_forward && forward_move)) {
                startIteration(forward_move, false);
            }
            break;
        case DONE:
            GEOPHILE_ASSERT(this->current().eof());
            return this->current();
    }
    if (_position >= 0 && _position < _record_array.nRecords()) {
        Record<SOR> record = _record_array.at(_position);
        _position += forward_move ? 1 : -1;
        current(record.key().z(), record.spatialObjectReference());
        this->state(IN_USE);
        _start_at = record.key();
    } else {
        this->close();
    }
    _forward = forward_move;
    return this->current();
}

template <typename SOR>
void RecordArrayCursor<SOR>::startIteration(int32_t forward_move, int32_t include_start_key)
{
    _position = _record_array.position(_start_at, forward_move, include_start_key);
}
