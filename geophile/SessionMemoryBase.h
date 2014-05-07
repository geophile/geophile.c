#ifndef _SESSION_MEMORY_BASE_H
#define _SESSION_MEMORY_BASE_H

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
        virtual ~SessionMemoryBase();

    protected:
        SessionMemoryBase();

    public: // Used internally. 
        // Friend declarations not used because these are also used in testing.
        ZArray* zArray();
        RegionPool* regions();

    private:
        ZArray* _zs;
        RegionPool* _regions;
    };
}

#endif
