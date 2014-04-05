#ifndef _SPATIAL_INDEX_SCAN_H
#define _SPATIAL_INDEX_SCAN_H

#include "Z.h"

namespace geophile
{
    class Cursor;
    class OrderedIndex;
    class SessionMemory;
    class SpatialIndexFilter;
    class SpatialObject;
    class SpatialObjectArray;

    class SpatialIndexScan
    {
    public:
        void find(Z z);
        ~SpatialIndexScan();
        SpatialIndexScan(OrderedIndex* index, 
                         const SpatialObject* query_object,
                         const SpatialIndexFilter* filter, 
                         SpatialObjectArray* output);

    private:
        OrderedIndex* _index;
        const SpatialObject* _query_object;
        const SpatialIndexFilter* _filter;
        SpatialObjectArray* _output;
        Cursor* _cursor;
    };
}

#endif
