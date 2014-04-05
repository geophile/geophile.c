#include "SessionMemory.h"
#include "SpatialObjectArray.h"
#include "RegionPool.h"
#include "ZArray.h"

using namespace geophile;

ZArray* SessionMemory::zArray()
{
    return _zs;
}

RegionPool* SessionMemory::regions()
{
    return _regions;
}

SpatialObjectArray* SessionMemory::output()
{
    return _output;
}

void SessionMemory::clearOutput()
{
    _output->clear();
}

SessionMemory::~SessionMemory()
{
    delete _output;
    delete _zs;
    delete _regions;
}

SessionMemory::SessionMemory()
    : _output(new SpatialObjectArray()),
      _zs(new ZArray()),
      _regions(new RegionPool())
{}
