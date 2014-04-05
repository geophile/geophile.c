#ifndef _SPATIAL_INDEX_FILTER_H
#define _SPATIAL_INDEX_FILTER_H

namespace geophile
{
    class SpatialObject;

    /*
     * A retrieval from a spatial index may yield a few false
     * positives. A SpatialObjectFilter is used to eliminate them.
     */
    class SpatialIndexFilter
    {
    public:
        /*
         * Returns true if query_object and spatial_object overlap,
         * false otherwise. query_object is the SpatialObject passed
         * to SpatialIndex::findOverlapping. spatial_object is one of
         * the objects contained in the SpatialIndex.
         */
        virtual bool overlap(const SpatialObject* query_object, 
                             const SpatialObject* spatial_object) const = 0;
    };
}

#endif
