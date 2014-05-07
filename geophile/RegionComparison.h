#ifndef _REGION_COMPARISON_H
#define _REGION_COMPARISON_H

namespace geophile
{
    typedef enum 
    { 
        REGION_OUTSIDE_OBJECT, 
        REGION_INSIDE_OBJECT, 
        REGION_OVERLAPS_OBJECT 
    } RegionComparison;
}

#endif
