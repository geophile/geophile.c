#include "SessionMemoryBase.h"
#include "RegionPool.h"
#include "ZArray.h"

using namespace geophile;

SessionMemoryBase::~SessionMemoryBase()
{
    delete _zs;
    delete _regions;
}

SessionMemoryBase::SessionMemoryBase()
    : _zs(new ZArray()),
      _regions(new RegionPool()),
      _buffer(NULL),
      _buffer_size(-1)
{}

ZArray* SessionMemoryBase::zArray()
{
    return _zs;
}

RegionPool* SessionMemoryBase::regions()
{
    return _regions;
}

ByteBuffer SessionMemoryBase::byteBuffer()
{
    return ByteBuffer(_buffer, _buffer_size);
}

void SessionMemoryBase::ensureByteBufferCapacity(uint32_t minimum)
{
    if (minimum > _buffer_size) {
        delete [] _buffer;
        _buffer = new byte[minimum];
        _buffer_size = minimum;
    }
}

uint32_t SessionMemoryBase::byteBufferCapacity()
{
    return _buffer_size;
}
