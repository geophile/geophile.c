#ifndef _SOR_H
#define _SOR_H

/*
 * A spatial object reference (SOR) must support the following operations:
 * - set to null
 * - test for null
 * - assignment (operator=)
 * - construction from a SpatialObject*
 * - return SpatialObject.id()
 *
 * To allow both SpatialObject* and classes to work as an SOR, the interface to SORs is
 * based on functions, not member functions. Here is the SOR interface:
 *
 *     bool isNull(const SOR&)
 *     void setNull(const SOR&)
 *     void operator=(const SOR&)
 *     SOR newSpatialObjectReference(const SpatialObject*)
 *     int64_t spatialObjectId(const SOR&)
 *
 * This module provides the functions allowing SpatialObject* to work as an SOR.
 */

#include "SpatialObject.h"

namespace geophile{
    inline bool isNull(const SpatialObject* spatial_object)
    {
        return spatial_object == NULL;
    }
    
    inline void setNull(SpatialObject*& spatial_object)
    {
        spatial_object = NULL;
    }
    
    // void operator= not needed for SpatialObject*}
    
    inline const SpatialObject* newSpatialObjectReference(const SpatialObject* spatial_object)
    {
        return spatial_object;
    }
    
    inline int64_t spatialObjectId(const SpatialObject* spatial_object)
    {
        return spatial_object->id();
    }
}

#endif
