#ifndef _SPATIAL_OBJECT_ARRAY_H
#define _SPATIAL_OBJECT_ARRAY_H

#include <stdint.h>

typedef int32_t (*SortFunction)(const void*, const void*);

namespace geophile
{
    class SpatialObject;

    /*
     * A SpatialObjectArray is used to accumulate the SpatialObjects
     * resulting from one or more retrievals from a SpatialIndex.  The
     * objects contained in a SpatialObjectArray are owned by it, and
     * are deleted by the destructor, and when clear() is called.
     */
    class SpatialObjectArray
    {
        friend class SessionMemory;

    public:
        /*
         * Returns the number of SpatialObjects in this SpatialObjectArray.
         */
        uint32_t length() const;
        /*
         * Returns the SpatialObject at the given position.
         */
        SpatialObject* at(uint32_t position) const;
        /*
         * Clears this SpatialObjectArray.
         */
        void clear();

    public: // Used internally and in testing
        ~SpatialObjectArray();
        SpatialObjectArray();
        void append(SpatialObject* spatial_object);

    public: // Used in testing
        void sort(SortFunction sort_function);

    private: // Part of the implementation
        void ensureSpace();

    private:
        static const uint32_t INITIAL_CAPACITY = 100;

    private:
        uint32_t _capacity;
        uint32_t _n;
        SpatialObject** _spatial_objects;
    };
}

#endif
