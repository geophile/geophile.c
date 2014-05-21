#ifndef _TESTBASE_H
#define _TESTBASE_H

#include "geophile.h"
#include "OrderedIndexFactory.h"

int runTests(geophile::OrderedIndexFactory<const geophile::SpatialObject*>* index_factory);

#endif
