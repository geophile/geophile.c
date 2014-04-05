#include "SpatialObjectTypes.h"
#include "Point2.h"
#include "Box2.h"
#include "util.h"

using namespace geophile;

static SpatialObject* uninitializedConstructor()
{
    GEOPHILE_ASSERT(false);
    return NULL;
}

SpatialObject* SpatialObjectTypes::newSpatialObject(uint32_t type_id) const
{
    GEOPHILE_ASSERT(type_id < _capacity);
    GEOPHILE_ASSERT(_constructors[type_id] != uninitializedConstructor);
    return _constructors[type_id]();
}

void SpatialObjectTypes::registerType(uint32_t type_id, SpatialObjectConstructor constructor)
{
    if (type_id >= _capacity) {
        uint32_t new_capacity = type_id + 1;
        SpatialObjectConstructor* new_constructors = new SpatialObjectConstructor[new_capacity];
        initializeConstructors(new_constructors, new_capacity);
        memcpy(new_constructors, _constructors, sizeof(SpatialObjectConstructor) * _capacity);
        delete [] _constructors;
        _constructors = new_constructors;
        _capacity = new_capacity;
    }
    _constructors[type_id] = constructor;
}

SpatialObjectTypes::~SpatialObjectTypes()
{
    delete [] _constructors;
}

SpatialObjectTypes::SpatialObjectTypes()
    : _capacity(INITIAL_SIZE),
      _constructors(new SpatialObjectConstructor[INITIAL_SIZE])
{
    initializeConstructors(_constructors, _capacity);
}

void SpatialObjectTypes::initializeConstructors(SpatialObjectConstructor* array, uint32_t n)
{
    for (uint32_t i = 0; i < n; i++) {
        array[i] = uninitializedConstructor;
    }
}
