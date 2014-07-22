#ifndef _IN_MEMORY_SPATIAL_OBJECT_MEMORY_MANAGER_H
#define _IN_MEMORY_SPATIAL_OBJECT_MEMORY_MANAGER_H

#include "SpatialObjectMemoryManager.h"
#include "SpatialObjectPointer.h"

namespace geophile
{
    class InMemorySpatialObjectMemoryManager : public SpatialObjectMemoryManager<SpatialObjectPointer>
    {
    public:
        virtual SpatialObjectPointer newSpatialObjectReference(const SpatialObject* spatial_object) const
        {
            return SpatialObjectPointer(spatial_object);
        }

        virtual const SpatialObject* spatialObject(const SpatialObjectPointer& sop) const
        {
            return sop.spatialObject();
        }
    };
}

#endif
