#ifndef _TESTSPATIALOBJECT_H
#define _TESTSPATIALOBJECT_H

#include "geophile/SpatialObject.h"

// Dummy SpatialObject for testing index

namespace geophile
{
    class TestSpatialObject : public SpatialObject
    {
    public: // SpatialObject
        virtual void id(int64_t id)
        {
            _id = id;
        }
        
        virtual int64_t id() const
        {
            return _id;
        }
        
        virtual void arbitraryPoint(double* coords) const
        {
            GEOPHILE_ASSERT(false);
        }
        
        virtual uint32_t maxZ() const
        {
            return 1;
        }
        
        virtual int32_t equalTo(const SpatialObject& spatialObject) const
        {
            return _id == spatialObject.id();
        }
        
        virtual int32_t containedBy(const Region* region) const
        {
            GEOPHILE_ASSERT(false);
            return 0;
        }
        
        virtual RegionComparison compare(const Region* region) const
        {
            GEOPHILE_ASSERT(false);
            return REGION_OUTSIDE_OBJECT;
        }
        
        virtual int32_t typeId() const
        {
            return TYPE_ID;
        }
        
        virtual void readFrom(ByteBuffer& byteBuffer)
        {
            _id = byteBuffer.getInt64();
        }
        
        virtual void writeTo(ByteBuffer& byteBuffer) const
        {
            byteBuffer.putInt64(_id);
        }
        
        virtual ~TestSpatialObject()
        {}
        
        virtual void copyFrom(const SpatialObject* spatial_object)
        {
            const TestSpatialObject* s = (const TestSpatialObject*) spatial_object;
            _id = s->_id;
        }

        virtual bool isNull() const
        {
            return _id == UNDEFINED_ID;
        }

        virtual void setNull()
        {
            _id = UNDEFINED_ID;
        }

    public: // TestSpatialObject
        TestSpatialObject(int64_t id)
            : _id(id)
        {}
        
        TestSpatialObject()
            : _id(0)
        {}

    public:
        static const uint32_t TYPE_ID = 0;

    private:
        static const int32_t UNDEFINED_ID = -1;
        
    private:
        int64_t _id;        
    };
}

#endif
