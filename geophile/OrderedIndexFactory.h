#ifndef _ORDERED_INDEX_FACTORY_H
#define _ORDERED_INDEX_FACTORY_H

namespace geophile
{
    template <typename SOR> class OrderedIndex;
    class SpatialObjectTypes;

    template <typename SOR>
    class OrderedIndexFactory
    {
    public:
        virtual OrderedIndex<SOR>* newIndex
        (const SpatialObjectTypes* spatial_object_types) const = 0;
    };
}

#endif
