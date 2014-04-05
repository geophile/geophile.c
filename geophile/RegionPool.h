#ifndef _REGION_POOL_H
#define _REGION_POOL_H

#include "Space.h"

namespace geophile
{
    class Region;
    class Space;

    class RegionPool
    {
    public:
        Region* takeRegion();
        void returnRegion(Region* region);
        ~RegionPool();
        RegionPool();

    private:
        uint32_t useFreePosition();

    private:
        static const uint32_t INITIAL_CAPACITY = 20;

        uint32_t _capacity;
        Region* _regions;
        bool* _free;
    };
}

#endif
