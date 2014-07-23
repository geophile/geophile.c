#ifndef _INLINE_SPATIAL_OBJECT_MEMORY_MANAGER_H
#define _INLINE_SPATIAL_OBJECT_MEMORY_MANAGER_H

#include "SpatialObjectMemoryManager.h"
#include "SpatialObjectPointer.h"

namespace geophile
{
    template <class SOR>
    class InlineSpatialObjectMemoryManager : public SpatialObjectMemoryManager<SOR>
    {
    public:
        virtual SOR newSpatialObjectReference(const SpatialObject* spatial_object) const
        {
            return SOR(spatial_object);
        }

        virtual void cleanupSpatialObjectReference(const SOR& sor) const
        {}
    };
}

#endif
