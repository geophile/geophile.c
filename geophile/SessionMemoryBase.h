#ifndef _SESSION_MEMORY_BASE_H
#define _SESSION_MEMORY_BASE_H

#include "ByteBuffer.h"

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
        ByteBuffer byteBuffer();
        void ensureByteBufferCapacity(uint32_t minimum);
        uint32_t byteBufferCapacity();

    private:
        ZArray* _zs;
        RegionPool* _regions;
        byte* _buffer;
        uint32_t _buffer_size;
    };
}

#endif
