#ifndef _REGION_QUEUE_H
#define _REGION_QUEUE_H

#include <stdint.h>

namespace geophile
{
    class Region;

    class RegionQueue
    {
    public:
        void add(Region* region);
        Region* take();
        uint32_t size();
        ~RegionQueue();
        RegionQueue(uint32_t capacity);

    private:
        uint32_t advance(uint32_t position) const;

    private:
        const uint32_t _capacity;
        Region** _regions;
        uint32_t _front;
        uint32_t _back;
        uint32_t _size;
    };
}

#endif
