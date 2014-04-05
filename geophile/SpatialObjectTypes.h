#ifndef _SPATIAL_OBJECT_TYPES_H
#define _SPATIAL_OBJECT_TYPES_H

#include <stdint.h>

namespace geophile
{
    class SpatialObject;

    typedef SpatialObject* (*SpatialObjectConstructor)();

    class SpatialObjectTypes
    {
        friend class OrderedIndex;
        
    public:
        /*
         * Register a SpatialObject constructor with a user-supplied type id.
         */
        void registerType(uint32_t type_id, SpatialObjectConstructor constructor);
        ~SpatialObjectTypes();
        SpatialObjectTypes();

    private:
        SpatialObject* newSpatialObject(uint32_t type_id) const;
        static void initializeConstructors(SpatialObjectConstructor* array, uint32_t n);

    private:
        static const uint32_t INITIAL_SIZE = 10;
        uint32_t _capacity;
        SpatialObjectConstructor* _constructors;
    };
}

#endif
