#ifndef _SPATIAL_OBJECT_POINTER_H
#define _SPATIAL_OBJECT_POINTER_H

namespace geophile
{
    class SpatialObjectPointer
    {
    public: // SpatialObject reference interface

        int64_t spatialObjectId() const
        {
            return _spatial_object->id();
        }

        bool isNull() const
        {
            return _spatial_object == NULL;
        }

        void setNull()
        {
            _spatial_object = NULL;
        }

        void operator=(const SpatialObjectPointer& sop)
        {
            _spatial_object = sop._spatial_object;
        }

        bool operator==(const SpatialObjectPointer& sop)
        {
            return _spatial_object == sop._spatial_object;
        }

        const SpatialObject* spatialObject() const
        {
            return _spatial_object;
        }

    public: // SpatialObjectPointer interface

        SpatialObjectPointer(const SpatialObjectPointer& sop)
            : _spatial_object(sop._spatial_object)
        {}

        SpatialObjectPointer(const SpatialObject* spatial_object)
        {
            _spatial_object = spatial_object;
        }

        SpatialObjectPointer()
            : _spatial_object(NULL)
        {}

    private:
        const SpatialObject* _spatial_object;
    };
}

#endif
