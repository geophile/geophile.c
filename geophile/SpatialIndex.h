#ifndef _SPATIAL_INDEX_H
#define _SPATIAL_INDEX_H

#include <stdint.h>

namespace geophile
{
    class OrderedIndex;
    class SessionMemory;
    class Space;
    class SpatialIndexFilter;
    class SpatialIndexScan;
    class SpatialObject;

    /*
     * A SpatialIndex organizes a set of SpatialObjects for the
     * efficient execution of spatial searches.
     */
    class SpatialIndex
    {
    public:
        /*
         * Space containing the indexed SpatialObjects.
         */
        const Space* space() const;
        /*
         * Adds spatial_object to this SpatialIndex. memory contains
         * resources used internally.
         */
        void add(const SpatialObject* spatial_object, SessionMemory* memory);
        /*
         * Removes spatial_object to this SpatialIndex. memory contains
         * resources used internally.
         */
        int32_t remove(const SpatialObject& spatial_object, SessionMemory* memory);
        /*
         * Prepares this SpatialIndex for retrieval.
         */
        void freeze();
        /*
         * Find all the SpatialObjects in this SpatialIndex that
         * overlap spatial_object.  Spatial index retrieval may return
         * false positives, which are removed by the filter.
         * The results are returned in memory->output(). 
         */
        void findOverlapping(const SpatialObject* query_object, 
                             const SpatialIndexFilter* filter,
                             SessionMemory* memory) const;
        /*
         * Constructor.
         *     space: The Space containing the SpatialObjects to be indexed.
         *     index: The OrderedIndex that will contain records of the spatial index.
         */
        SpatialIndex(const Space* space, OrderedIndex* index);

    public: // Not part of the API. Public for testing.
        SpatialIndexScan* newScan(const SpatialObject* query_object,
                                  const SpatialIndexFilter* filter, 
                                  SessionMemory* memory) const;

    private:
        const Space* _space;
        OrderedIndex* _index;
    };
}

#endif
