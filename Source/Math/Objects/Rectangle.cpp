#include "Framework.h"
#include "Math/Objects/Rectangle.h"

namespace Silent::Math
{
    bool Rectangle::Intersects(const Vector2& point) const
    {
        return point.x >= (Center.x - Extents.x) && point.x <= (Center.x + Extents.x) &&
               point.y >= (Center.y - Extents.y) && point.y <= (Center.y + Extents.y);
    }
}
