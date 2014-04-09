#include "SpatialIndexScan.h"
#include "OrderedIndex.h"
#include "Cursor.h"
#include "Record.h"
#include "SessionMemory.h"
#include "SpatialIndexFilter.h"
#include "SpatialObjectKey.h"
#include "SpatialObjectArray.h"

using namespace geophile;

template <typename SOR>
void SpatialIndexScan<SOR>::find(Z z)
{
    if (!_cursor) {
        _cursor = _index->cursor();
    }
    int64_t zhi = z.hi();
    _cursor->goTo(SpatialObjectKey(z));
    Record<SOR> record = _cursor->next();
    while (!record.eof() && record.key().z().asInteger() < zhi) {
        SpatialObject* spatial_object = record.spatialObject();
        if (_filter->overlap(_query_object, spatial_object)) {
            _output->append(spatial_object);
        } else {
            delete record.spatialObject();
        }
        record = _cursor->next();
    }
    if (!record.eof()) {
        delete record.spatialObject();
    }
}

template <typename SOR>
SpatialIndexScan<SOR>::~SpatialIndexScan()
{
    delete _cursor;
}

template <typename SOR>
SpatialIndexScan<SOR>::SpatialIndexScan(OrderedIndex<SOR>* index, 
                                        const SpatialObject* query_object,
                                        const SpatialIndexFilter* filter, 
                                        SpatialObjectArray* output)
    : _index(index),
      _query_object(query_object),
      _filter(filter),
      _output(output),
      _cursor(NULL)
{}
