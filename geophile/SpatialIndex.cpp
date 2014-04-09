#include "Space.h"
#include "SpatialIndex.h"
#include "SpatialObject.h"
#include "ZArray.h"
#include "OrderedIndex.h"
#include "SessionMemory.h"
#include "SpatialIndexScan.h"
#include "util.h"

using namespace geophile;

template <typename SOR>
const Space* SpatialIndex<SOR>::space() const
{
    return _space;
}

template <typename SOR>
void SpatialIndex<SOR>::add(const SpatialObject* spatial_object, SessionMemory* memory)
{
    GEOPHILE_ASSERT(spatial_object->id() != SpatialObject::UNINITIALIZED_ID);
    ZArray* zs = memory->zArray();
    zs->clear();
    _space->decompose(spatial_object, spatial_object->maxZ(), memory);
    for (uint32_t i = 0; i < zs->length(); i++) {
        _index->add(zs->at(i), spatial_object);
    }
}

template <typename SOR>
int32_t SpatialIndex<SOR>::remove(const SpatialObject& spatial_object, SessionMemory* memory)
{
    // TBD
    GEOPHILE_ASSERT(false); 
    return false;
}

template <typename SOR>
void SpatialIndex<SOR>::freeze()
{
    _index->freeze();
}

template <typename SOR>
SpatialIndexScan<SOR>* SpatialIndex<SOR>::newScan(const SpatialObject* query_object,
                                                  const SpatialIndexFilter* filter, 
                                                  SessionMemory* memory) const
{
    return new SpatialIndexScan<SOR>(_index, query_object, filter, memory->output());
}

template <typename SOR>
void SpatialIndex<SOR>::findOverlapping(const SpatialObject* query_object, 
                                        const SpatialIndexFilter* filter,
                                        SessionMemory* memory) const
{
    _space->decompose(query_object, query_object->maxZ(), memory);
    SpatialIndexScan<SOR>* scan = newScan(query_object, filter, memory);
    ZArray* zs = memory->zArray();
    for (uint32_t i = 0; i < zs->length(); i++) {
        scan->find(zs->at(i));
    }
    delete scan;
}

template <typename SOR>
SpatialIndex<SOR>::SpatialIndex(const Space* space, OrderedIndex<SOR>* index)
    : _space(space),
      _index(index)
{}
