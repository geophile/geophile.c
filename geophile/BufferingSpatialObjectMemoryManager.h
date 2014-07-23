#ifndef _BUFFERING_SPATIAL_OBJECT_MEMORY_MANAGER_H
#define _BUFFERING_SPATIAL_OBJECT_MEMORY_MANAGER_H

#include "SpatialObjectMemoryManager.h"
#include "SpatialObjectTypes.h"

namespace geophile
{
    template <class SOR>
    class BufferingSpatialObjectMemoryManager : public SpatialObjectMemoryManager<SOR>
    {
    public: // SpatialObjectMemoryManager interface
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

    public: // BufferingSpatialObjectMemoryManager interface
        BufferingSpatialObjectMemoryManager(const SpatialObjectTypes* spatial_object_types)
            : _spatial_object_types(spatial_object_types)
        {}

    private:
        const SpatialObjectTypes* _spatial_object_types;
    };
}

#endif
