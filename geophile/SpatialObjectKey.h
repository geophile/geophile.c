#ifndef _SPATIAL_OBJECT_KEY_H
#define _SPATIAL_OBJECT_KEY_H

#include "Z.h"

namespace geophile
{
    class SpatialObjectKey
    {
    public:
        void operator=(const SpatialObjectKey& k)
        {
            _z = k._z;
            _soid = k._soid;
        }

        Z z() const
        {
            return _z;
        }
        
        int64_t soid() const
        {
            return _soid;
        }
        
        int32_t compare(const SpatialObjectKey& k) const
        {
            return
                _z < k._z ? -1 : 
                _z > k._z ? 1 : 
                _soid < k._soid ? -1 : 
                _soid > k._soid ? 1 : 0;
        }

        void set(Z z, int64_t soid)
        {
            _z = z;
            _soid = soid;
        }
        
        SpatialObjectKey(Z z)
            : _z(z),
              _soid(MIN_SOID)
        {}

        SpatialObjectKey(Z z, int64_t soid)
            : _z(z),
              _soid(soid)
        {}

        SpatialObjectKey(const SpatialObjectKey& k)
            : _z(k._z),
              _soid(k._soid)
        {}

        SpatialObjectKey()
            : _z(),
              _soid(-1)
        {}
    
    private:
        static const int64_t MIN_SOID = 0x8000000000000000LL;

    private:
        Z _z;
        int64_t _soid;
    };
}

#endif
