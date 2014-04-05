#include "ByteBuffer.h"
#include "ByteBufferUnderflowException.h"
#include "ByteBufferOverflowException.h"
#include "util.h"

using namespace geophile;

int32_t ByteBuffer::getInt32()
{
    if (_position + sizeof(int32_t) > _end) {
        throw ByteBufferUnderflowException(NULL);
    }
    int32_t target;
    memcpy(&target, _position, sizeof(int32_t));
    _position += sizeof(int32_t);
    checkState();
    return target;
}

int64_t ByteBuffer::getInt64()
{
    if (_position + sizeof(int64_t) > _end) {
        throw ByteBufferUnderflowException(NULL);
    }
    int64_t target;
    memcpy(&target, _position, sizeof(int64_t));
    _position += sizeof(int64_t);
    checkState();
    return target;
}

uint32_t ByteBuffer::getUint32()
{
    if (_position + sizeof(uint32_t) > _end) {
        throw ByteBufferUnderflowException(NULL);
    }
    uint32_t target;
    memcpy(&target, _position, sizeof(uint32_t));
    _position += sizeof(uint32_t);
    checkState();
    return target;
}

uint64_t ByteBuffer::getUint64()
{
    if (_position + sizeof(uint64_t) > _end) {
        throw ByteBufferUnderflowException(NULL);
    }
    uint64_t target;
    memcpy(&target, _position, sizeof(uint64_t));
    _position += sizeof(uint64_t);
    checkState();
    return target;
}

double ByteBuffer::getDouble()
{
    if (_position + sizeof(double) > _end) {
        throw ByteBufferUnderflowException(NULL);
    }
    double target;
    memcpy(&target, _position, sizeof(double));
    _position += sizeof(double);
    checkState();
    return target;
}

void ByteBuffer::getBytes(byte* target, uint32_t count)
{
    if (_position + count > _end) {
        throw ByteBufferUnderflowException(NULL);
    }
    memcpy(&target, _position, count);
    _position += count;
    checkState();
}

ByteBuffer& ByteBuffer::putInt32(int32_t source)
{
    if (_position + sizeof(int32_t) > _end) {
        throw ByteBufferOverflowException(NULL);
    }
    memcpy(_position, &source, sizeof(int32_t));
    _position += sizeof(int32_t);
    checkState();
    return *this;
}

ByteBuffer& ByteBuffer::putInt64(int64_t source)
{
    if (_position + sizeof(int64_t) > _end) {
        throw ByteBufferOverflowException(NULL);
    }
    memcpy(_position, &source, sizeof(int64_t));
    _position += sizeof(int64_t);
    checkState();
    return *this;
}

ByteBuffer& ByteBuffer::putUint32(uint32_t source)
{
    if (_position + sizeof(uint32_t) > _end) {
        throw ByteBufferOverflowException(NULL);
    }
    memcpy(_position, &source, sizeof(uint32_t));
    _position += sizeof(uint32_t);
    checkState();
    return *this;
}

ByteBuffer& ByteBuffer::putUint64(uint64_t source)
{
    if (_position + sizeof(uint64_t) > _end) {
        throw ByteBufferOverflowException(NULL);
    }
    memcpy(_position, &source, sizeof(uint64_t));
    _position += sizeof(uint64_t);
    checkState();
    return *this;
}

ByteBuffer& ByteBuffer::putDouble(double source)
{
    if (_position + sizeof(double) > _end) {
        throw ByteBufferOverflowException(NULL);
    }
    memcpy(_position, &source, sizeof(double));
    _position += sizeof(double);
    checkState();
    return *this;
}

ByteBuffer& ByteBuffer::putBytes(byte* source, uint32_t count)
{
    if (_position + count > _end) {
        throw ByteBufferOverflowException(NULL);
    }
    memcpy(_position, &source, count);
    _position += count;
    checkState();
    return *this;
}

uint32_t ByteBuffer::capacity() const
{
    return _end - _start;
}

uint32_t ByteBuffer::limit() const
{
    return _limit - _start;
}

uint32_t ByteBuffer::position() const
{
    return _position - _start;
}

uint32_t ByteBuffer::remaining() const
{
    return _end - _position;
}

ByteBuffer& ByteBuffer::clear()
{
    _limit = _end;
    _position = _start;
    checkState();
    return *this;
}

ByteBuffer& ByteBuffer::flip()
{
    _limit = _position;
    _position = _start;
    checkState();
    return *this;
}

ByteBuffer& ByteBuffer::limit(uint32_t limit)
{
    _limit = _start + limit;
    maintainMark();
    checkState();
    return *this;
}

ByteBuffer& ByteBuffer::position(uint32_t position)
{
    _position = _start + position;
    maintainMark();
    checkState();
    return *this;
}

ByteBuffer& ByteBuffer::reset()
{
    GEOPHILE_ASSERT(_mark != NULL);
    _position = _mark;
    checkState();
    return *this;
}

ByteBuffer& ByteBuffer::rewind()
{
    _position = _start;
    maintainMark();
    checkState();
    return *this;
}

ByteBuffer& ByteBuffer::mark()
{
    _mark = _position;
    checkState();
    return *this;
}

void ByteBuffer::copyTo(byte* target)
{
    uint32_t n = remaining();
    memcpy(target, _position, n);
    _position += n;
    maintainMark();
    checkState();
}

ByteBuffer::~ByteBuffer()
{}

ByteBuffer::ByteBuffer(byte* bytes, uint32_t length)
    : _start(bytes),
      _end(bytes + length),
      _position(bytes),
      _limit(bytes + length),
      _mark(NULL)
{
    checkState();
}

void ByteBuffer::checkState() const
{
    if (_mark != NULL) {
        GEOPHILE_ASSERT(_start <= _mark);
        GEOPHILE_ASSERT(_mark <= _position);
    }
    GEOPHILE_ASSERT(_position <= _limit);
    GEOPHILE_ASSERT(_limit <= _end);
}

void ByteBuffer::maintainMark()
{
    if (_position < _mark || _limit < _mark) {
        _mark = NULL;
    }
}
