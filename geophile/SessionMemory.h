#ifndef _SESSION_MEMORY_H
#define _SESSION_MEMORY_H

namespace geophile
{
    class RegionPool;
    class Space;
    class SpatialObject;
    class SpatialObjectArray;
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
        SpatialObjectArray* output();
        /*
         * Clears the internally-maintained SpatialObjectArray returned by output().
         */
        void clearOutput();
        /*
         * Destructor
         */
        ~SessionMemory();
        /*
         * Constructor
         */
        SessionMemory();

    public: // Used internally. 
        // Friend declarations not used because these are also used in testing.
        ZArray* zArray();
        RegionPool* regions();

    private:
        SpatialObjectArray* _output;
        ZArray* _zs;
        RegionPool* _regions;
    };
}

#endif
