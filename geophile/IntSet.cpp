#include "IntSet.h"
#include "util.h"

using namespace geophile;

static const double LOAD_FACTOR = 0.5;

void IntSet::add(int64_t x)
{
    uint32_t p = position(x);
    if (!_occupied[p]) {
        GEOPHILE_ASSERT(_count < _capacity);
        _array[p] = x;
        _occupied[p] = true;
        _count++;
    }
}

int32_t IntSet::contains(int64_t x) const
{
    return _occupied[position(x)];
}

uint32_t IntSet::count() const
{
    return _count;
}

IntSet::~IntSet()
{
    delete [] _array;
    delete [] _occupied;
}

IntSet::IntSet(uint32_t capacity)
    : _capacity(capacity),
      _slots((uint32_t)(capacity / LOAD_FACTOR)),
      _count(0)
{
    _array = new int64_t[_slots];
    _occupied = new uint8_t[_slots];
    memset(_occupied, 0, sizeof(int8_t) * _slots);
}

uint32_t IntSet::position(int64_t x) const
{
    int64_t xp = x * PRIME;
    if (xp < 0) {
        xp = -xp;
    }
    uint32_t position = xp % _slots;
    while (_occupied[position] && _array[position] != x) {
        position = (position + 1) % _slots;
    }
    return position;
}
