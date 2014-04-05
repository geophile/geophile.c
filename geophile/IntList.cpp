#include "IntList.h"
#include "util.h"

using namespace geophile;

void IntList::append(int64_t x)
{
    GEOPHILE_ASSERT(_count < _size);
    _array[_count++] = x;
}

uint32_t IntList::count() const
{
    return _count;
}

int64_t IntList::at(uint32_t position) const
{
    GEOPHILE_ASSERT(position < _count);
    return _array[position];
}

IntList::~IntList()
{
    delete [] _array;
}

IntList::IntList(uint32_t size)
    : _size(size),
      _count(0),
      _array(new int64_t[size])
{}
