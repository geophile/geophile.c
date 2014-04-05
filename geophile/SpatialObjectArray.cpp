#include <stdlib.h>
#include "SpatialObjectArray.h"
#include "SpatialObject.h"
#include "util.h"

using namespace geophile;

void SpatialObjectArray::append(SpatialObject* spatial_object)
{
    ensureSpace();
    _spatial_objects[_n++] = spatial_object;
}

uint32_t SpatialObjectArray::length() const
{
    return _n;
}

SpatialObject* SpatialObjectArray::at(uint32_t position) const
{
    GEOPHILE_ASSERT(position < _n);
    return _spatial_objects[position];
}

void SpatialObjectArray::sort(SortFunction sort_function)
{
    qsort(_spatial_objects, _n, sizeof(SpatialObject*), sort_function);
}

void SpatialObjectArray::clear()
{
    for (uint32_t i = 0; i < _n; i++) {
        delete _spatial_objects[i];
    }
    _n = 0;
}

SpatialObjectArray::~SpatialObjectArray()
{
    for (uint32_t i = 0; i < _n; i++) {
        delete _spatial_objects[i];
    }
    delete [] _spatial_objects;
}

SpatialObjectArray::SpatialObjectArray()
    : _capacity(INITIAL_CAPACITY),
      _n(0),
      _spatial_objects(new SpatialObject*[INITIAL_CAPACITY])
{}

void SpatialObjectArray::ensureSpace()
{
    if (_n == _capacity) {
        uint32_t new_capacity = _capacity * 2;
        SpatialObject** new_spatial_objects = new SpatialObject*[new_capacity];
        memcpy(new_spatial_objects, _spatial_objects, sizeof(SpatialObject*) * _capacity);
        delete [] _spatial_objects;
        _capacity = new_capacity;
        _spatial_objects = new_spatial_objects;
    }
}
