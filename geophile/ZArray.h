#ifndef _Z_ARRAY_H
#define _Z_ARRAY_H

#include "Z.h"
#include "Space.h"

namespace geophile
{
    class ZArray
    {
    public:
        Z at(uint32_t i) const;
        void set(uint32_t i, Z z);
        void append(Z z);
        uint32_t length() const;
        void sort();
        void remove(uint32_t position);
        void clear();
        ~ZArray();
        ZArray();

    private:
        void ensureSpace();
        // For use with qsort
        static int32_t zcompare(const void*, const void*);

    private:
        static const uint32_t INITIAL_CAPACITY = Space::MAX_DIMENSIONS * 2;

        uint32_t _capacity;
        uint32_t _n;
        Z* _z;
    };
}

#endif
