#ifndef _INLINE_POINT2_H
#define _INLINE_POINT2_H

#include <geophile/Point2.h>

namespace geophile
{
    class InlinePoint2
    {
    public:
        void set(const Point2* point)
        {
            _point = *point;
        }

        bool isNull() const
        {
            return _point.isNull();
        }
    
        void setNull()
        {
            _point.setNull();
        }
    
        const Point2* point2() const
        {
            return &_point;
        }
    
        int64_t spatialObjectId() const
        {
            return _point.id();
        }

        void operator=(const InlinePoint2& p)
        {
            _point = p._point;
        }

        InlinePoint2(const Point2& p)
            : _point(p)
        {}

        InlinePoint2()
        {}

    private:
        Point2 _point;
    };

    namespace SpatialObjectReference
    {
        inline void initializeSpatialObjectReference(InlinePoint2& inlinePoint, 
                                                     const SpatialObject* spatial_object)
        {
            const Point2* point = (const Point2*) spatial_object;
            inlinePoint.set(point);
        }
        
        inline bool isNull(const InlinePoint2& inlinePoint)
        {
            return inlinePoint.isNull();
        }
        
        inline void setNull(InlinePoint2& inlinePoint)
        {
            inlinePoint.setNull();
        }
        
        inline int64_t spatialObjectId(const InlinePoint2& inlinePoint)
        {
            return inlinePoint.spatialObjectId();
        }
        
        inline const Point2* spatialObject(const InlinePoint2& inlinePoint)
        {
            return inlinePoint.point2();
        }
        
        inline void deleteSpatialObject(const InlinePoint2& spatial_object)
        {}
    }
}

#endif
