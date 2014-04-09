#ifndef _RECORD_H
#define _RECORD_H

#include "SpatialObject.h"
#include "SpatialObjectKey.h"
#include "Z.h"

namespace geophile
{
    class SpatialObject;

    template <typename SOR> // SOR: Spatial Object Reference
    class Record
    {
    public:
        Record<SOR> operator=(const Record<SOR>& record)
        {
            if (record.eof()) {
                setEOF();
            } else {
                set(record.key().z(), record.spatialObject());
            }
            return *this;
        }

        const SpatialObjectKey& key() const
        {
            return _key;
        }

        SOR spatialObject() const
        {
            return _spatial_object_reference;
        }

        int32_t eof() const
        {
            return isNull(_spatial_object_reference);
        }

        void setEOF()
        {
            _key = Z();
            setNull(_spatial_object_reference);
        }

        void set(Z z, SOR spatial_object_reference)
        {
            GEOPHILE_ASSERT(!isNull(spatial_object_reference));
            _key = SpatialObjectKey(z, spatialObjectId(spatial_object_reference));
            _spatial_object_reference = spatial_object_reference;
        }

        Record()
            : _key()
        {
            setNull(_spatial_object_reference);
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
