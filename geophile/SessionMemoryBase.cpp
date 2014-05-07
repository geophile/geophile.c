#include "SessionMemoryBase.h"
#include "RegionPool.h"
#include "ZArray.h"

using namespace geophile;

SessionMemoryBase::~SessionMemoryBase()
{
    delete _zs;
    delete _regions;
}

SessionMemoryBase::SessionMemoryBase()
    : _zs(new ZArray()),
      _regions(new RegionPool())
{}

ZArray* SessionMemoryBase::zArray()
{
    return _zs;
}

RegionPool* SessionMemoryBase::regions()
{
    return _regions;
}
