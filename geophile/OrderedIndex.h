#ifndef _ORDERED_INDEX_H
#define _ORDERED_INDEX_H

#include "Z.h"
#include "SpatialObjectTypes.h"
#include "ByteBuffer.h"
#include "ByteBufferOverflowException.h"

namespace geophile
{
    template <class SOR> class Cursor;
    template <class SOR> class SessionMemory;
    template <class SOR> class SpatialObjectMemoryManager;
    class SpatialObject;

    /*
     * When a SpatialObject, s, is added to a SpatialIndex, records of
     * the form ((z, id), s) are added to an Index. The key, (z, id),
     * comprises a z-value z, and the spatial object's id. (The SpatialIndex
     * is responsible for generating z-values and calling OrderedIndex::add.)
     * Removal of a SpatialObject from a SpatialIndex is done by calling
     * remove for each z-value of the SpatialObject. In order to completely
     * remove a SpatialObject, the same z-values must be provided as when the
     * object was added. 
     *
     * Before an OrderedIndex can be used for retrieval, freeze() must
     * be called. Then, access to OrderedIndex contents is
     * accomplished using a Cursor, obtained by cursor().
     */
    template <class SOR> // SOR: Spatial Object Reference
        class OrderedIndex
    {
    public:
        /*
         * Adds ((z, spatial_object->id()), spatial_object) to this
         * OrderedIndex, where spatial_object = sor.spatialObject().
         */
        virtual void add(Z z, const SOR& sor) = 0;
        /*
         * Removes from the OrderedIndex the record keyed by (z,
         * soid).  Returns the SOR that was located and removed.  If
         * no record was removed, then for the SOR returned, sor,
         * isNull(sor) is true..
         */
        virtual SOR remove(Z z, int64_t soid) = 0;
        /*
         * Prepare the index for retrievals. Depending on the
         * implementation, an OrderedIndex may be searchable before
         * freeze, and add/remove may work after the call..
         */
        virtual void freeze() = 0;
        /*
         * Return a Cursor object, which can be used for retrieval
         * from this OrderedIndex..
         */
        virtual Cursor<SOR>* cursor() = 0;
        /*
         * Destructor
         */
        virtual ~OrderedIndex() {}
        /*
         * Constructor. 
         */
        OrderedIndex(const SpatialObjectTypes* spatial_object_types, 
                     SessionMemory<SOR>* memory,
                     const SpatialObjectMemoryManager<SOR>* spatial_object_memory_manager)
            : _spatial_object_types(spatial_object_types),
              _memory(memory),
              _spatial_object_memory_manager(spatial_object_memory_manager)
        {}

    protected:
        /*
         * Serializes spatial_object.
         */
        ByteBuffer serialize(const SpatialObject* spatial_object)
        {
            ByteBuffer buffer = _memory->byteBuffer();
            int32_t serialized = false;
            try {
                spatial_object->writeTo(buffer);
                serialized = true;
            } catch (ByteBufferOverflowException e) {
                _memory->ensureBufferCapacity(_memory->bufferCapacity() * 2);
            }
        }

        /*
         * Used by OrderedIndex subclasses to create a new
         * SpatialObject of the type registered with the given
         * type_id.
         */
        SpatialObject* newSpatialObject(uint32_t type_id) const
        {
            return _spatial_object_types->newSpatialObject(type_id);
        }

    protected:
        SessionMemory<SOR>* _memory;
        const SpatialObjectMemoryManager<SOR>* _spatial_object_memory_manager;

    private:
        const SpatialObjectTypes* _spatial_object_types;
    };
}

#endif
