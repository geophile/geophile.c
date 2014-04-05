#ifndef _POINT2_H
#define _POINT2_H

#include <stdint.h>
#include <stddef.h>
#include "SpatialObject.h"

namespace geophile
{
    class Point2 : public SpatialObject
    {
    public:
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
