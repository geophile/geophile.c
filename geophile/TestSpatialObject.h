#ifndef _TEST_SPATIAL_OBJECT_H
#define _TEST_SPATIAL_OBJECT_H

#include "RegionComparison.h"
#include "SpatialObject.h"

namespace geophile
{
    class SpatialObject;
    class Region;
    class ByteBuffer;

    class TestSpatialObject : public SpatialObject
    {
    public: // SpatialObject
        virtual void id(int64_t id);
        virtual int64_t id() const;
        virtual void arbitraryPoint(double* coords) const;
        virtual uint32_t maxZ() const;
        virtual int32_t equalTo(const SpatialObject& spatialObject) const;
        virtual int32_t containedBy(const Region* region) const;
        virtual RegionComparison compare(const Region* region) const;
        virtual int32_t typeId() const;
        virtual void readFrom(ByteBuffer& byteBuffer);
        virtual void writeTo(ByteBuffer& byteBuffer) const;
        virtual ~TestSpatialObject();
        virtual void copyFrom(const SpatialObject* spatial_object);
        virtual bool isNull() const;
        virtual void setNull();
    public: // TestSpatialObject
        TestSpatialObject(int64_t id);
        TestSpatialObject();

    public:
        static const uint32_t TYPE_ID = 0;

    private:
        static const int32_t UNDEFINED_ID = -1;
        
    private:
        int64_t _id;        
    };
}

#endif
