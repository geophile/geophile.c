#include "RegionQueue.h"
#include "util.h"

using namespace geophile;

void RegionQueue::add(Region* region)
{
    GEOPHILE_ASSERT(_size < _capacity);
    if (_size == 0) {
        _front = 0;
        _back = 0;
    } 
    _regions[_back] = region;
    _back = advance(_back);
    _size++;
}

Region* RegionQueue::take()
{
    GEOPHILE_ASSERT(_size > 0);
    Region* region = _regions[_front];
    _regions[_front] = NULL;
    _front = advance(_front);
    _size--;
    return region;
}

uint32_t RegionQueue::size()
{
    return _size;
}

RegionQueue::~RegionQueue()
{
    delete [] _regions;
}

RegionQueue::RegionQueue(uint32_t capacity)
    : _capacity(capacity),
      _regions(new Region*[capacity + 1]),
      _size(0)
{}

uint32_t RegionQueue::advance(uint32_t position) const
{
    return (position + 1) % (_capacity + 1);
}
