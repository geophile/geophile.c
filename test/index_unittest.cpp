#include <assert.h>
#include <stdio.h>

#include "testbase.h"
#include "RecordArray.h"

using namespace geophile;

class RecordArrayFactory : public OrderedIndexFactory<const geophile::SpatialObject*>
{
public:
    virtual OrderedIndex<const SpatialObject*>* newIndex
    (const SpatialObjectTypes* spatial_object_types) const
    {
        return new RecordArray<const SpatialObject*>(spatial_object_types);
    }
};

RecordArrayFactory RECORD_ARRAY_FACTORY;

int main(int32_t argc, const char** argv)
{
    runTests(&RECORD_ARRAY_FACTORY);
}
