#ifndef _INLINE_SPATIAL_OBJECT_REFERENCE_MANAGER_H
#define _INLINE_SPATIAL_OBJECT_REFERENCE_MANAGER_H

#include "SpatialObjectReferenceManager.h"
#include "SpatialObjectPointer.h"

namespace geophile
{
    template <class SOR>
    class InlineSpatialObjectReferenceManager : public SpatialObjectReferenceManager<SOR>
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
