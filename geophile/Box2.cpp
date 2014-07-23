#include <math.h>
#include "Space.h"
#include "Region.h"
#include "Box2.h"
#include "ByteBuffer.h"
#include "util.h"

using namespace geophile;

void Box2::id(int64_t id)
{
    _id = id;
}

int64_t Box2::id() const
{
    return _id;
}

void Box2::arbitraryPoint(double* coords) const
{
    coords[0] = _xlo;
    coords[1] = _ylo;
}

uint32_t Box2::maxZ() const
{
    return MAX_Z;
}

int32_t Box2::equalTo(const SpatialObject& spatial_object) const
{
    const Box2& box = (const Box2&) spatial_object;
    return
        _xlo == box._xlo &&
        _xhi == box._xhi &&
        _ylo == box._ylo &&
        _yhi == box._yhi;
}

int32_t Box2::containedBy(const Region* region) const
{
    const Space* space = region->space();
    uint64_t zxlo = space->appToZ(0, _xlo);
    uint64_t zxhi = space->appToZ(0, _xhi);
    uint64_t zylo = space->appToZ(1, _ylo);
    uint64_t zyhi = space->appToZ(1, _yhi);
    return
        region->lo(0) <= zxlo && zxhi <= region->hi(0) &&
        region->lo(1) <= zylo && zyhi <= region->hi(1);
}

RegionComparison Box2::compare(const Region* region) const
{
    uint64_t rxlo = region->lo(0);
    uint64_t rylo = region->lo(1);
    uint64_t rxhi = region->hi(0);
    uint64_t ryhi = region->hi(1);
    const Space* space = region->space();
    uint64_t zxlo = space->appToZ(0, _xlo);
    uint64_t zxhi = space->appToZ(0, _xhi);
    uint64_t zylo = space->appToZ(1, _ylo);
    uint64_t zyhi = space->appToZ(1, _yhi);
    if (zxlo <= rxlo && rxhi <= zxhi && zylo <= rylo && ryhi <= zyhi) {
        return REGION_INSIDE_OBJECT;
    } else if (rxhi < zxlo || rxlo > zxhi || ryhi < zylo || rylo > zyhi) {
        return REGION_OUTSIDE_OBJECT;
    } else {
        return REGION_OVERLAPS_OBJECT;
    }
}

int32_t Box2::typeId() const
{
    return TYPE_ID;
}

void Box2::readFrom(ByteBuffer& byte_buffer)
{
    _id = byte_buffer.getInt64();
    _xlo = byte_buffer.getDouble();
    _xhi = byte_buffer.getDouble();
    _ylo = byte_buffer.getDouble();
    _yhi = byte_buffer.getDouble();
}

void Box2::writeTo(ByteBuffer& byte_buffer) const
{
    byte_buffer.putInt64(_id);
    byte_buffer.putDouble(_xlo);
    byte_buffer.putDouble(_xhi);
    byte_buffer.putDouble(_ylo);
    byte_buffer.putDouble(_yhi);
}

void Box2::copyFrom(const SpatialObject* spatial_object)
{
    GEOPHILE_ASSERT(typeId() == spatial_object->typeId());
    const Box2* box = (const Box2*) spatial_object;
    _id = box->_id;
    _xlo = box->_xlo;
    _xhi = box->_xhi;
    _ylo = box->_ylo;
    _yhi = box->_yhi;
}

bool Box2::isNull() const
{
    return isnan(_xlo);
}

void Box2::setNull()
{
    _xlo = NAN;
}

double Box2::xlo() const
{
    return _xlo;
}

double Box2::xhi() const
{
    return _xhi;
}

double Box2::ylo() const
{
    return _ylo;
}

double Box2::yhi() const
{
    return _yhi;
}

Box2::Box2(double xlo, double xhi, double ylo, double yhi)
    : _id(UNINITIALIZED_ID),
      _xlo(xlo),
      _xhi(xhi),
      _ylo(ylo),
      _yhi(yhi)
{}

Box2::Box2()
    : _id(UNINITIALIZED_ID),
      _xlo(0),
      _xhi(0),
      _ylo(0),
      _yhi(0)
{}
