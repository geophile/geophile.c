#ifndef _SESSION_MEMORY_H
#define _SESSION_MEMORY_H

#include "SpatialObjectArrayBase.h"
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
    class SessionMemory
    {
    public:
        /*
         * Returns an internally-maintained SpatialObjectArray that accumulates output
         * from SpatialIndex retrievals.
         */
        SpatialObjectArrayBase* output()
        {
            return _output;
        }

        /*
         * Clears the internally-maintained SpatialObjectArray returned by output().
         */
        void clearOutput()
        {
            _output->clear();
        }

        /*
         * Destructor
         */
        ~SessionMemory()
        {
            delete _zs;
            delete _regions;
        }

        /*
         * Constructor
         */
        SessionMemory(SpatialObjectArrayBase* output)
            : _output(output),
              _zs(new ZArray()),
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
        SpatialObjectArrayBase* _output;
        ZArray* _zs;
        RegionPool* _regions;
    };
}

#endif
