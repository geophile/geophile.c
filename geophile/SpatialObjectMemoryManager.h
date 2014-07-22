#ifndef _SPATIAL_OBJECT_MEMORY_MANAGER_H
#define _SPATIAL_OBJECT_MEMORY_MANAGER_H

namespace geophile
{
    template <class SOR>
    class SpatialObjectMemoryManager
    {
    public:
        virtual SOR newSpatialObjectReference(const SpatialObject*) const = 0;
        virtual void cleanup(SOR sor) const = 0;
    };
}

#endif
