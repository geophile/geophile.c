#include <assert.h>
#include <stdio.h>

#include "ByteBuffer.h"
#include "TestSpatialObject.h"
#include "util.h"

using namespace geophile;

void TestSpatialObject::id(int64_t id)
{
    _id = id;
}

int64_t TestSpatialObject::id() const
{
    return _id;
}

void TestSpatialObject::arbitraryPoint(double* coords) const
{
    GEOPHILE_ASSERT(false);
}

uint32_t TestSpatialObject::maxZ() const
{
    return 1;
}

int32_t TestSpatialObject::equalTo(const SpatialObject& spatialObject) const
{
    return _id == spatialObject.id();
}

int32_t TestSpatialObject::containedBy(const Region* region) const
{
    GEOPHILE_ASSERT(false);
    return 0;
}

RegionComparison TestSpatialObject::compare(const Region* region) const
{
    GEOPHILE_ASSERT(false);
    return REGION_OUTSIDE_OBJECT;
}

int32_t TestSpatialObject::typeId() const
{
    return TYPE_ID;
}

void TestSpatialObject::readFrom(ByteBuffer& byteBuffer)
{
    _id = byteBuffer.getInt64();
}

void TestSpatialObject::writeTo(ByteBuffer& byteBuffer) const
{
    byteBuffer.putInt64(_id);
}

TestSpatialObject::~TestSpatialObject()
{}

void TestSpatialObject::copyFrom(const SpatialObject* spatial_object)
{
    const TestSpatialObject* s = (const TestSpatialObject*) spatial_object;
    _id = s->_id;
}

bool TestSpatialObject::isNull() const
{
    return _id == UNDEFINED_ID;
}

void TestSpatialObject::setNull()
{
    _id = UNDEFINED_ID;
}

TestSpatialObject::TestSpatialObject(int64_t id)
    : _id(id)
{}

TestSpatialObject::TestSpatialObject()
    : _id(0)
{}
