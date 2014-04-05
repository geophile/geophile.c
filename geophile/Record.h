#ifndef _RECORD_H
#define _RECORD_H

#include "SpatialObject.h"
#include "SpatialObjectKey.h"
#include "Z.h"

namespace geophile
{
    class SpatialObject;

    class Record
    {
    public:
        Record operator=(const Record& record)
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

        SpatialObject* spatialObject() const
        {
            return _spatial_object;
        }

        int32_t eof() const
        {
            return _eof;
        }

        void setEOF()
        {
            _key = Z();
            _spatial_object = NULL;
            _eof = true;
        }

        void set(Z z, SpatialObject* spatial_object)
        {
            _key = SpatialObjectKey(z, spatial_object->id());
            _spatial_object = spatial_object;
            _eof = false;
        }

        Record()
            : _key(),
              _spatial_object(NULL),
              _eof(true)
        {}

        Record(const Record& record)
            : _key(record._key),
              _spatial_object(record._spatial_object),
              _eof(record._eof)
        {}

    private:
        SpatialObjectKey _key;
        SpatialObject* _spatial_object;
        int32_t _eof;
    };
}

#endif
