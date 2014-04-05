#ifndef _INT_SET_H
#define _INT_SET_H

#include <stdint.h>

namespace geophile
{
    class IntSet
    {
    public:
        void add(int64_t x);
        int32_t contains(int64_t x) const;
        uint32_t count() const;
        ~IntSet();
        IntSet(uint32_t capacity);

    private:
        uint32_t position(int64_t x) const;

    private:
        static const int64_t PRIME = 9987001;

    private:
        const uint32_t _capacity;
        const uint32_t _slots;
        uint32_t _count;
        int64_t* _array;
        uint8_t* _occupied;
    };
}

#endif
