#ifndef _SPATIAL_OBJECT_MEMORY_MANAGER_H
#define _SPATIAL_OBJECT_MEMORY_MANAGER_H

namespace geophile
{
    template <class SOR>
    class SpatialObjectMemoryManager
    {
    public:
        // Create a new reference to the given SpatialObject. This may involve making 
        // a heap-based copy of the SpatialObject, copying SpatialObject state into the 
        // reference, etc. The returned reference may be copied, (e.g. if
        // SOR::operator==(const SOR&) const is defined), but in creating such a copy,
        // newSpatialObjectReference is not called. In other words, side-effects
        // by newSpatialObjectReference only occur when the original
        // reference is created.
        virtual SOR newSpatialObjectReference(const SpatialObject*) const = 0;

        // Do any cleanup required when sor is no longer needed. This function
        // is applied to the references in an OrderedIndex being
        // destroyed. This provides an opportunity to reverse any side-effects by
        // newSpatialObjectReference. Needless to say, copies of sor must not
        // be used after cleanupSpatialObjectReference is called.
        virtual void cleanupSpatialObjectReference(const SOR& sor) const = 0;
    };
}

#endif
