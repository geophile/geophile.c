#ifndef _SPATIAL_OBJECT_ARRAY_H
#define _SPATIAL_OBJECT_ARRAY_H

#include <stdint.h>
#include <stdlib.h>
#include "SpatialObjectArrayBase.h"
#include "util.h"

typedef int32_t (*SortFunction)(const void*, const void*);

namespace geophile
{
    class SpatialObject;

    /*
     * A SpatialObjectArray is used to accumulate the SpatialObjects
     * resulting from one or more retrievals from a SpatialIndex.
     */
    template <typename SOR> class SpatialObjectArray : public SpatialObjectArrayBase
    {
    public:
        /*
         * Returns the element at the given position.
         */
        SOR at(uint32_t position) const
        {
            GEOPHILE_ASSERT(position < _n);
            return _contents[position];
        }

    public: // Used internally and in testing
        virtual ~SpatialObjectArray()
        {
            delete [] _contents;
        }

        SpatialObjectArray()
            : _contents(new SOR[INITIAL_CAPACITY])
        {}

        void append(SOR sor)
        {
            ensureSpace();
            _contents[_n++] = sor;
        }

    public: // Used in testing
        void sort(SortFunction sort_function)
        {
            qsort(_contents, _n, sizeof(SOR), sort_function);
        }

    private: // Part of the implementation
        void ensureSpace()
        {
            if (_n == _capacity) {
                uint32_t new_capacity = _capacity * 2;
                SOR* new_contents = new SOR[new_capacity];
                memcpy(new_contents, _contents, sizeof(SOR) * _capacity);
                delete [] _contents;
                _capacity = new_capacity;
                _contents = new_contents;
            }
        }

    private:
        SOR* _contents;
    };
}

#endif
