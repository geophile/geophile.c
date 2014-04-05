#include "Space.h"
#include "SpatialIndex.h"
#include "SpatialObject.h"
#include "ZArray.h"
#include "OrderedIndex.h"
#include "SessionMemory.h"
#include "SpatialIndexScan.h"
#include "util.h"

using namespace geophile;

const Space* SpatialIndex::space() const
{
    return _space;
}

void SpatialIndex::add(const SpatialObject* spatial_object, SessionMemory* memory)
{
    GEOPHILE_ASSERT(spatial_object->id() != SpatialObject::UNINITIALIZED_ID);
    ZArray* zs = memory->zArray();
    zs->clear();
    _space->decompose(spatial_object, spatial_object->maxZ(), memory);
    for (uint32_t i = 0; i < zs->length(); i++) {
        _index->add(zs->at(i), spatial_object);
    }
}

int32_t SpatialIndex::remove(const SpatialObject& spatial_object, SessionMemory* memory)
{
    // TBD
    GEOPHILE_ASSERT(false); 
    return false;
}

void SpatialIndex::freeze()
{
    _index->freeze();
}

SpatialIndexScan* SpatialIndex::newScan(const SpatialObject* query_object,
                                        const SpatialIndexFilter* filter, 
                                        SessionMemory* memory) const
{
    return new SpatialIndexScan(_index, query_object, filter, memory->output());
}

void SpatialIndex::findOverlapping(const SpatialObject* query_object, 
                                   const SpatialIndexFilter* filter,
                                   SessionMemory* memory) const
{
    _space->decompose(query_object, query_object->maxZ(), memory);
    SpatialIndexScan* scan = newScan(query_object, filter, memory);
    ZArray* zs = memory->zArray();
    for (uint32_t i = 0; i < zs->length(); i++) {
        scan->find(zs->at(i));
    }
    delete scan;
}

SpatialIndex::SpatialIndex(const Space* space, OrderedIndex* index)
    : _space(space),
      _index(index)
{}
