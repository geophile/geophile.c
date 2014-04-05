#include "RegionPool.h"
#include "Region.h"
#include "util.h"

using namespace geophile;

Region* RegionPool::takeRegion() 
{
    return &_regions[useFreePosition()];
}

void RegionPool::returnRegion(Region* region)
{
    GEOPHILE_ASSERT(region >= _regions);
    GEOPHILE_ASSERT(region < _regions + _capacity);
    uint32_t p = region - _regions;
    _free[p]= true;
}

RegionPool::~RegionPool()
{
    delete [] _regions;
    delete [] _free;
}

RegionPool::RegionPool()
    : _capacity(INITIAL_CAPACITY),
      _regions(new Region[INITIAL_CAPACITY]),
      _free(new bool[INITIAL_CAPACITY])
{
    for (int p = 0; p < INITIAL_CAPACITY; p++) {
        _free[p] = true;
    }
}

uint32_t RegionPool::useFreePosition()
{
    uint32_t freePosition = _capacity;
    for (uint32_t p = 0; freePosition == _capacity && p < _capacity; p++) {
        if (_free[p]) {
            freePosition = p;
        }
    }
    if (freePosition == _capacity) {
        uint32_t new_capacity = _capacity * 2;
        // Grow _regions
        Region* new_regions = new Region[new_capacity];
        memcpy(new_regions, _regions, _capacity * sizeof(Region));
        delete [] _regions;
        _regions = new_regions;
        // Grow _free
        bool* new_free = new bool[new_capacity];
        memcpy(new_free, _free, _capacity * sizeof(bool));
        delete [] _free;
        _free = new_free;
        //
        _capacity = new_capacity;
    }
    _free[freePosition] = false;
    return freePosition;
}
