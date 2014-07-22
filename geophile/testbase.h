#ifndef _TESTBASE_H
#define _TESTBASE_H

#include "geophile.h"
#include "OrderedIndexFactory.h"
#include "SpatialObjectPointer.h"

int runTests(geophile::OrderedIndexFactory<geophile::SpatialObjectPointer>* index_factory);

#endif
