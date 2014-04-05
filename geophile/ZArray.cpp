#include <stdlib.h>
#include "ZArray.h"

using namespace geophile;

Z ZArray::at(uint32_t i) const
{
    GEOPHILE_ASSERT(i < _n);
    return _z[i];
}

void ZArray::set(uint32_t i, Z z)
{
    GEOPHILE_ASSERT(i < _n);
    _z[i] = z;
}

void ZArray::append(Z z)
{
    ensureSpace();
    _z[_n++] = z;
}

uint32_t ZArray::length() const
{
    return _n;
}

void ZArray::sort()
{
    qsort(_z, _n, sizeof(Z), zcompare);
}

void ZArray::remove(uint32_t position)
{
    memmove(&_z[position], &_z[position + 1], sizeof(Z) * (_n - position - 1));
    _z[--_n].reset();
}

void ZArray::clear()
{
    _n = 0;
}

ZArray::~ZArray()
{
    delete [] _z;
}

ZArray::ZArray()
    : _capacity(INITIAL_CAPACITY),
      _n(0),
      _z(new Z[INITIAL_CAPACITY])
{}

void ZArray::ensureSpace()
{
    GEOPHILE_ASSERT(_n <= _capacity);
    if (_n == _capacity) {
        uint32_t new_capacity = _capacity * 2;
        Z* new_z = new Z[new_capacity];
        memcpy(new_z, _z, _capacity * sizeof(Z));
        delete [] _z;
        _z = new_z;
        _capacity = new_capacity;
    }
    GEOPHILE_ASSERT(_n < _capacity);
}

int32_t ZArray::zcompare(const void* x, const void* y)
{
    int64_t a = *(int64_t*)x;
    int64_t b = *(int64_t*)y;
    return a < b ? -1 : a > b ? 1 : 0;
}

