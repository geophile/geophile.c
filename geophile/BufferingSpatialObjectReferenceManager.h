#ifndef _BUFFERING_SPATIAL_OBJECT_REFERENCE_MANAGER_H
#define _BUFFERING_SPATIAL_OBJECT_REFERENCE_MANAGER_H

#include "SpatialObjectReferenceManager.h"
#include "SpatialObjectTypes.h"

namespace geophile
{
    template <class SOR>
    class BufferingSpatialObjectReferenceManager : public SpatialObjectReferenceManager<SOR>
    {
    public: // SpatialObjectReferenceManager interface
        virtual SOR newSpatialObjectReference(const SpatialObject* spatial_object) const
        {
            SpatialObject* copy = _spatial_object_types->newSpatialObject(spatial_object->typeId());
            copy->copyFrom(spatial_object);
            return SOR(copy);
        }

        virtual void cleanupSpatialObjectReference(const SOR& sor) const
        {
            delete sor.spatialObject();
        }

    public: // BufferingSpatialObjectReferenceManager interface
        BufferingSpatialObjectReferenceManager(const SpatialObjectTypes* spatial_object_types)
            : _spatial_object_types(spatial_object_types)
        {}

    private:
        const SpatialObjectTypes* _spatial_object_types;
    };
}

#endif
