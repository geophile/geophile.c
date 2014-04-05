#ifndef _INT_LIST_H
#define _INT_LIST_H

#include <stdint.h>

namespace geophile
{
    class IntList
    {
    public:
        void append(int64_t x);
        uint32_t count() const;
        int64_t at(uint32_t position) const;
        ~IntList();
        IntList(uint32_t size);

    private:
        const uint32_t _size;
        uint32_t _count;
        int64_t* _array;
    };
}

#endif
