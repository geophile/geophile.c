#ifndef _IN_MEMORY_SPATIAL_OBJECT_REFERENCE_MANAGER_H
#define _IN_MEMORY_SPATIAL_OBJECT_REFERENCE_MANAGER_H

#include "SpatialObjectReferenceManager.h"
#include "SpatialObjectPointer.h"

/*
 * SpatialObjectReferenceManager for heap-based SpatialObjects that are shared by the application
 * and the OrderedIndex. An inserted SpatialObject is not copied -- the identical object is 
 * referenced from the OrderedIndex. SpatialObjects are owned by the application and are not
 * deleted when the OrderedIndex is destroyed.
 */

namespace geophile
{
    class InMemorySpatialObjectReferenceManager : public SpatialObjectReferenceManager<SpatialObjectPointer>
    {
    public:
        virtual SpatialObjectPointer newSpatialObjectReference(const SpatialObject* spatial_object) const
        {
            return SpatialObjectPointer(spatial_object);
        }

        virtual void cleanupSpatialObjectReference(const SpatialObjectPointer& p) const
        {}
    };
}

#endif
