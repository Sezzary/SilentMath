#include "Framework.h"
#include "Math/Objects/Plane.h"

namespace Silent::Math
{
    bool Plane::operator==(const Plane& plane) const
    {
        return Normal == plane.Normal && Distance == plane.Distance;
    }

    bool Plane::operator!=(const Plane& plane) const
    {
        return !(*this == plane);
    }
}
