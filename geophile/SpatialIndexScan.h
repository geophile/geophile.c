#ifndef _SPATIAL_INDEX_SCAN_H
#define _SPATIAL_INDEX_SCAN_H

#include "Z.h"
#include "SpatialIndexScan.h"
#include "OrderedIndex.h"
#include "Cursor.h"
#include "Record.h"
#include "SessionMemory.h"
#include "SpatialIndexFilter.h"
#include "SpatialObjectKey.h"
#include "OutputArray.h"

namespace geophile
{
    template <class SOR> class Cursor;
    template <class SOR> class OrderedIndex;
    template <class SOR> class OutputArray;
    class SpatialIndexFilter;
    class SpatialObject;

    template <class SOR> class SpatialIndexScan
    {
    public:
        void find(Z z)
        {
            if (!_cursor) {
                _cursor = _index->cursor();
            }
            int64_t zhi = z.hi();
            _cursor->goTo(SpatialObjectKey(z));
            Record<SOR> record = _cursor->next();
            while (!record.eof() && record.key().z().asInteger() < zhi) {
                SOR spatial_object_reference = record.spatialObjectReference();
                const SpatialObject* spatial_object = spatial_object_reference.spatialObject();
                if (_filter->overlap(_query_object, spatial_object)) {
                    _output->append(spatial_object_reference);
                }
                record = _cursor->next();
            }
        }

        ~SpatialIndexScan()
        {
            delete _cursor;
        }

        SpatialIndexScan(OrderedIndex<SOR>* index, 
                         const SpatialObject* query_object,
                         const SpatialIndexFilter* filter, 
                         SpatialObjectMemoryManager<SOR>* spatial_object_memory_manager,
                         OutputArray<SOR>* output)
            : _index(index),
            _query_object(query_object),
            _filter(filter),
            _output(output),
            _cursor(NULL)
            {}

    private:
        OrderedIndex<SOR>* _index;
        const SpatialObject* _query_object;
        const SpatialIndexFilter* _filter;
        OutputArray<SOR>* _output;
        Cursor<SOR>* _cursor;
    };
}

#endif
