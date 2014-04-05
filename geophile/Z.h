#ifndef _Z_H
#define _Z_H

#include <stdint.h>
#include <stddef.h>
#include "util.h"

namespace geophile
{
    class Z
    {
    public:
        int32_t operator==(Z that) const
        {
            return this->_z == that._z;
        }

        int32_t operator!=(Z that) const
        {
            return this->_z != that._z;
        }

        int32_t operator<(Z that) const
        {
            return this->_z < that._z;
        }

        int32_t operator<=(Z that) const
        {
            return this->_z <= that._z;
        }

        int32_t operator>(Z that) const
        {
            return this->_z > that._z;
        }

        int32_t operator>=(Z that) const
        {
            return this->_z >= that._z;
        }

        void operator=(const Z& z)
        {
            this->_z = z._z;
        }

        // The low point of this z-value.
        int64_t lo() const
        {
            return _z;
        }

        // The high point of this z-value.
        int64_t hi() const
        {
            return _z | (((1L << (MAX_Z_BITS - length())) - 1) << LENGTH_BITS);
        }

        int64_t asInteger() const
        {
            return _z;
        }

        uint32_t length() const
        {
            return _z & LENGTH_MASK;
        }

        int32_t siblingOf(Z that) const
        {
            int32_t siblings = false;
            int32_t length = this->length();
            if (length > 0 && length == that.length()) {
                int64_t match_mask = ((1L << (length - 1)) - 1) << (64 - length);
                int64_t differ_mask = 1L << (63 - length);
                siblings = 
                    (this->_z & match_mask) == (that._z & match_mask) && 
                    (this->_z & differ_mask) != (that._z & differ_mask);
            }
            return siblings;
        }

        Z parent() const
        {
            uint32_t this_length = length();
            GEOPHILE_ASSERT(this_length > 0);
            uint32_t parent_length = this_length - 1;
            int64_t parent_mask = ((1L << parent_length) - 1) << (63 - parent_length);
            return Z(_z & parent_mask, parent_length);
        }

        int32_t contains(Z that) const
        {
            uint32_t this_length = this->length();
            uint32_t that_length = that.length();
            int32_t prefix = this_length <= that_length;
            if (prefix) {
                int64_t mask = ((1L << this_length) - 1) << (63 - this_length);
                prefix = (this->_z & mask) == (that._z & mask);
            }
            return prefix;
        }

        void reset()
        {
            _z = UNINITIALIZED;
        }

        Z(int64_t bits, uint32_t length)
            : _z(bits | length)
        {
            GEOPHILE_ASSERT(bits >= 0);
            GEOPHILE_ASSERT(length <= MAX_Z_BITS);
        }

        Z(const Z& z)
            : _z(z._z)
        {}

        Z()
            : _z(UNINITIALIZED)
        {}

        // MSB is unused. 6 LSBs contain the number of z-value bits.
        static const uint32_t LENGTH_BITS = 6;
        static const int64_t LENGTH_MASK = (1 << LENGTH_BITS) - 1;
        static const uint32_t MAX_Z_BITS = 57; 
        static const int64_t Z_MIN = 0x0L;
        static const int64_t Z_MAX = ((1L << MAX_Z_BITS) - 1) << LENGTH_BITS | LENGTH_MASK;
        
    private:

        static const int64_t UNINITIALIZED = -1L;

    private:
        int64_t _z;
    };
}

#endif
