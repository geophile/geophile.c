#ifndef _RECORD_H
#define _RECORD_H

#include "SpatialObject.h"
#include "SpatialObjectKey.h"
#include "Z.h"

namespace geophile
{
    class SpatialObject;

    template <class SOR> // SOR: Spatial Object Reference
    class Record
    {
    public:
        Record<SOR> operator=(const Record<SOR>& record)
        {
            if (record.eof()) {
                setEOF();
            } else {
                set(record.key().z(), record.spatialObjectReference());
            }
            return *this;
        }

        const SpatialObjectKey& key() const
        {
            return _key;
        }

        SOR spatialObjectReference() const
        {
            return _spatial_object_reference;
        }

        int32_t eof() const
        {
            return _spatial_object_reference.isNull();
        }

        void setEOF()
        {
            _key = Z();
            _spatial_object_reference.setNull();
        }

        void set(Z z, SOR spatial_object_reference)
        {
            GEOPHILE_ASSERT(!spatial_object_reference.isNull());
            _key = 
                SpatialObjectKey(z, 
                                 spatial_object_reference.spatialObjectId());
            _spatial_object_reference = spatial_object_reference;
        }

        Record()
            : _key()
        {
            _spatial_object_reference.setNull();
        }

        Record(const Record& record)
            : _key(record._key),
              _spatial_object_reference(record._spatial_object_reference)
        {}

    private:
        SpatialObjectKey _key;
        SOR _spatial_object_reference;
    };
}

#endif
