#ifndef _POINT2_H
#define _POINT2_H

#include <stdint.h>
#include <stddef.h>
#include "SpatialObject.h"

namespace geophile
{
    class Point2 : public SpatialObject
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
        virtual void copyFrom(const SpatialObject* spatial_object);
        virtual bool isNull() const;
        virtual void setNull();

    public: // SpatialObject reference interface
        int64_t spatialObjectId() const;
        /* Already declared by SpatialObject
        bool isNull() const;
        void setNull();
        */
        void operator=(const Point2& p);
        bool operator==(const Point2& p);
        const SpatialObject* spatialObject() const;
        Point2(const SpatialObject* spatial_object);

    public: // Point2
        double x() const;
        double y() const;
        Point2(double x, double y);
        Point2();

    public:
        static const int32_t TYPE_ID = 1;

    private:
        int64_t _id; // TODO: Move this to parent class
        double _x;
        double _y;
    };
}

#endif
