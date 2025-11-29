#include "Framework.h"
#include "Math/Objects/AxisAlignedBoundingRect.h"

namespace Silent::Math
{
    AxisAlignedBoundingRect::AxisAlignedBoundingRect(const Vector2& center, const Vector2& extents)
    {
        Center  = center;
        Extents = extents;
    }

    bool AxisAlignedBoundingRect::Intersects(const Vector2& point) const
    {
        return point.x >= (Center.x - Extents.x) && point.x <= (Center.x + Extents.x) &&
               point.y >= (Center.y - Extents.y) && point.y <= (Center.y + Extents.y);
    }
}
