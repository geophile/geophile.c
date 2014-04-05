#ifndef _REGION_H
#define _REGION_H

#include <stdint.h>
#include "Z.h"
#include "Space.h"

namespace geophile
{
    class Space;

    class Region
    {
    public:
        const Space* space() const;
        uint32_t level() const;
        uint64_t lo(uint32_t d) const;
        uint64_t hi(uint32_t d) const;
        uint32_t isPoint() const;
        // For use by this package
        void downLeft();
        void downRight();
        void up();
        Z z() const;
        void initialize(const Space* space, 
                        const uint64_t* lo, 
                        const uint64_t* hi, 
                        uint32_t level);
        void copyFrom(const Region* region);
        Region();

    private:
        Region(const Region& region);
        void checkInitialized() const;

    private:
        static const int64_t ONE = 1;

    private:
        const Space* _space;
        const uint32_t* _interleave;
        uint32_t _level;
        uint64_t _lo[Space::MAX_DIMENSIONS];
        uint64_t _hi[Space::MAX_DIMENSIONS];
        uint32_t _x_bit_position[Space::MAX_DIMENSIONS];
    };

    typedef enum 
    { 
        REGION_OUTSIDE_OBJECT, 
        REGION_INSIDE_OBJECT, 
        REGION_OVERLAPS_OBJECT 
    } RegionComparison;
}

#endif
