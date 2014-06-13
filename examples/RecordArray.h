#ifndef _RECORDARRAY_H
#define _RECORDARRAY_H

#include <geophile/OrderedIndex.h>
#include <geophile/Record.h>
#include <geophile/Cursor.h>
#include <geophile/SpatialObjectTypes.h>
#include <geophile/ByteBuffer.h>

namespace geophile
{
    class SpatialObjectKey;
    class SpatialObjectTypes;

    template <typename SOR> // SOR: Spatial Object Reference
    class RecordArray : public OrderedIndex<SOR>
    {
    public:
        // Index
        virtual void add(Z z, const SOR& sor);
        virtual SOR remove(Z z, int64_t soid);
        virtual void freeze();
        virtual Cursor<SOR>* cursor();
        virtual ~RecordArray();

        // RecordArray
        // If return value is negative, _start_key is missing, 
        // and insert position is -returnvalue - 1.
        int32_t position(const SpatialObjectKey& key, 
                         int32_t forward_move, 
                         int32_t include_key) const;
        uint32_t nRecords() const;
        Record<SOR> at(int32_t position) const;
        RecordArray(const SpatialObjectTypes* spatial_object_types, SessionMemory<SOR>* memory);
       
    private:
        // serialize and deserialize use _buffer (which is why this class
        // is not threadsafe).
        void serialize(const SpatialObject* spatial_object);        
        SpatialObject* deserialize();
        void growBuffer();
        void growArray();
        
    private:
        static int32_t recordCompare(const void* x, const void* y);
        
    private:
        static const uint32_t INITIAL_CAPACITY = 1000;
        static const uint32_t INITIAL_BUFFER_SIZE = 1000;
        
    private:
        int32_t _n;
        int32_t _capacity;
        Record<SOR>* _records;
        uint32_t _buffer_size;
        byte* _buffer;
    };

    template <typename SOR>
    class RecordArrayCursor : public Cursor<SOR>
    {
    public:
        virtual Record<SOR> next();
        virtual Record<SOR> previous();
        virtual void goTo(const SpatialObjectKey& key);
        RecordArrayCursor(RecordArray<SOR>& record_array);
        
    private:
        Record<SOR> neighbor(int32_t forward_move);
        void startIteration(int32_t forward_move, int32_t include_start_key);
        
    private:
        RecordArray<SOR>& _record_array;
        int32_t _position;
        SpatialObjectKey _start_at;
        int32_t _forward;
    };
}

// So that the functions can be instantiated
#include "RecordArray.cpp.h"

#endif
