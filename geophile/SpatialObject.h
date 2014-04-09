#ifndef _SPATIAL_OBJECT_H
#define _SPATIAL_OBJECT_H

#include <stdint.h>
#include "Region.h"

namespace geophile
{
    class ByteBuffer;
    class Region;

    class SpatialObject
    {
    public:
        virtual void id(int64_t id) = 0;
        virtual int64_t id() const = 0;
        virtual void arbitraryPoint(double* coords) const = 0;
        virtual uint32_t maxZ() const = 0;
        virtual int32_t equalTo(const SpatialObject& spatialObject) const = 0;
        virtual int32_t containedBy(const Region* region) const = 0;
        virtual RegionComparison compare(const Region* region) const = 0;
        virtual int32_t typeId() const = 0;
        virtual void readFrom(ByteBuffer& byteBuffer) = 0;
        virtual void writeTo(ByteBuffer& byteBuffer) const = 0;
        virtual ~SpatialObject() {}

    public:
        static const int64_t UNINITIALIZED_ID = -1LL;
    };
}

// So that users of SpatialObjects can use SpatialObject* as a spatial
// object reference.
#include "SOR.h"

#endif
