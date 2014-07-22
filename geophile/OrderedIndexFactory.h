#ifndef _ORDERED_INDEX_FACTORY_H
#define _ORDERED_INDEX_FACTORY_H

namespace geophile
{
    template <class SOR> class OrderedIndex;
    class SpatialObjectTypes;

    template <class SOR>
    class OrderedIndexFactory
    {
    public:
        virtual OrderedIndex<SOR>* newIndex
            (const SpatialObjectTypes* spatial_object_types) const = 0;

        virtual ~OrderedIndexFactory()
        {}
    };
}

#endif
