#include "Space.h"
#include "Region.h"
#include "Point2.h"
#include "ByteBuffer.h"

using namespace geophile;

void Point2::id(int64_t id)
{
    _id = id;
}

int64_t Point2::id() const
{
    return _id;
}

void Point2::arbitraryPoint(double* coords) const
{
    coords[0] = _x;
    coords[1] = _y;
}

uint32_t Point2::maxZ() const
{
    return 1;
}

int32_t Point2::equalTo(const SpatialObject& spatial_object) const
{
    const Point2& point = (const Point2&) spatial_object;
    return
        _x == point._x &&
        _y == point._y;
}

int32_t Point2::containedBy(const Region* region) const
{
    const Space* space = region->space();
    uint64_t zx = space->appToZ(0, _x);
    uint64_t zy = space->appToZ(1, _y);
    return
        region->lo(0) <= zx && zx <= region->hi(0) &&
        region->lo(1) <= zy && zy <= region->hi(1);
}

RegionComparison Point2::compare(const Region* region) const
{
    uint64_t rxlo = region->lo(0);
    uint64_t rylo = region->lo(1);
    uint64_t rxhi = region->hi(0);
    uint64_t ryhi = region->hi(1);
    const Space* space = region->space();
    uint64_t zx = space->appToZ(0, _x);
    uint64_t zy = space->appToZ(1, _y);
    if (rxlo <= zx && zx <= rxhi &&
        rylo <= zy && zy <= ryhi) {
        return REGION_INSIDE_OBJECT;
    } else {
        return REGION_OUTSIDE_OBJECT;
    }
}

int32_t Point2::typeId() const
{
    return TYPE_ID;
}

void Point2::readFrom(ByteBuffer& byte_buffer)
{
    _x = byte_buffer.getDouble();
    _y = byte_buffer.getDouble();
}

void Point2::writeTo(ByteBuffer& byte_buffer) const
{
    byte_buffer.putDouble(_x);
    byte_buffer.putDouble(_y);
}

double Point2::x() const
{
    return _x;
}

double Point2::y() const
{
    return _y;
}

Point2::Point2(double x, double y)
    : _id(UNINITIALIZED_ID),
      _x(x),
      _y(y)
{}

Point2::Point2()
    : _id(UNINITIALIZED_ID),
      _x(0),
      _y(0)
{}
