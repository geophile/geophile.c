#ifndef _BOX2_H
#define _BOX2_H

#include <stdint.h>
#include <stddef.h>
#include "SpatialObject.h"

namespace geophile
{
    class Box2 : public SpatialObject
    {
    public: // SpatialObject
        virtual void id(int64_t id);
        virtual int64_t id() const;
        virtual void arbitraryPoint(double* coords) const;
        virtual uint32_t maxZ() const;
        virtual int32_t equalTo(const SpatialObject& spatial_object) const;
        virtual int32_t containedBy(const Region* region) const;
        virtual RegionComparison compare(const Region* region) const;
        virtual int32_t typeId() const;
        virtual void readFrom(ByteBuffer& byte_buffer);
        virtual void writeTo(ByteBuffer& byte_buffer) const;
        virtual void copyFrom(const SpatialObject* spatial_object);
        virtual bool isNull() const;
        virtual void setNull();

    public: // Box2
        double xlo() const;
        double xhi() const;
        double ylo() const;
        double yhi() const;
        Box2(double xlo, double xhi, double ylo, double yhi);
        Box2();

    public:
        static const int32_t TYPE_ID = 2;

    private:
        static const uint32_t MAX_Z = 8;

    private:
        int64_t _id; // TODO: Move this to parent class
        double _xlo;
        double _xhi;
        double _ylo;
        double _yhi;
    };
}

#endif
