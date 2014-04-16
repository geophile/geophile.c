#ifndef _SESSION_MEMORY_BASE_H
#define _SESSION_MEMORY_BASE_H

#include "ZArray.h"
#include "RegionPool.h"

namespace geophile
{
    class RegionPool;
    class Space;
    class SpatialObject;
    class ZArray;

    /*
     * Maintains memory used in various SpatialIndex operations,
     * avoiding repeated allocation/deallocation..
     */
    class SessionMemoryBase
    {
    public:
        /*
         * Destructor
         */
        virtual ~SessionMemoryBase()
        {
            delete _zs;
            delete _regions;
        }

    protected:
        SessionMemoryBase()
            : _zs(new ZArray()),
              _regions(new RegionPool())
        {}

    public: // Used internally. 
        // Friend declarations not used because these are also used in testing.
        ZArray* zArray()
        {
            return _zs;
        }

        RegionPool* regions()
        {
            return _regions;
        }

    private:
        ZArray* _zs;
        RegionPool* _regions;
    };
}

#endif
