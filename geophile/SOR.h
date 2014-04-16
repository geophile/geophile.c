#ifndef _SOR_H
#define _SOR_H

/*
 * A spatial object reference (SOR) must support the following operations:
 * - Construction with no arguments (to allow for SOR arrays).
 * - Initialization given const SpatialObject*.
 * - set to "null" state.
 * - test for null state.
 * - return SpatialObject.id()
 * - return SpatialObject*
 * - delete SpatialObject
 * - copy constructor: operator=(const SOR&)
 *
 * A SOR can be either a SpatialObject* or a class. For this reason,
 * the interface to SORs is based on functions, not member functions,
 * (except for the copy constructor).
 *
 * This module provides the functions allowing SpatialObject* to work as an SOR.
 */

#include "SpatialObject.h"

namespace geophile
{
    inline void initializeSpatialObjectReference(const SpatialObject*& sor, 
                                                 const SpatialObject* spatial_object)
    {
        sor = spatial_object;
    }

    inline bool isNull(const SpatialObject* sor)
    {
        return sor == NULL;
    }
    
    inline void setNull(const SpatialObject*& sor)
    {
        sor = NULL;
    }
    
    inline int64_t spatialObjectId(const SpatialObject* sor)
    {
        return sor->id();
    }

    inline const SpatialObject* spatialObject(const SpatialObject* sor)
    {
        return sor;
    }

    inline void deleteSpatialObject(const SpatialObject* sor)
    {
        delete (SpatialObject*) sor;
    }
}

#endif
