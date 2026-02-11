#pragma once

#include "Math/Objects/Vector3.h"
#include "Math/Objects/Quaternion.h"

namespace Silent::Math
{
    class      AxisAlignedBoundingBox;
    class      BoundingSphere;
    class      OrientedBoundingBox;
    enum class ContainmentType;

    /** @brief 3D bounding frustum. */
    class BoundingFrustum
    {
    public:
        // =======
        // Fields
        // =======

        Vector3    Origin      = Vector3::Zero;
        Quaternion Rotation    = Quaternion::Identity;
        float      RightSlope  = 0.0f;
        float      LeftSlope   = 0.0f;
        float      TopSlope    = 0.0f;
        float      BottomSlope = 0.0f;
        float      Near        = 0.0f;
        float      Far         = 0.0f;

        // =============
        // Constructors
        // =============

        /** @brief Creates a default instance. */
        constexpr BoundingFrustum() = default;

        /** @brief Creates an instance from frustum components.
         *
         * @param origin Frustum origin.
         * @param rot Frustum rotation.
         * @param rightSlope Right slope of the frustum.
         * @param leftSlope Left slope of the frustum.
         * @param topSlope Top slope of the frustum.
         * @param bottomSlope Bottom slope of the frustum.
         * @param near Near plane distance.
         * @param far Far plane distance.
         */
        constexpr BoundingFrustum(const Vector3& origin, const Quaternion& rot,
                                  float rightSlope, float leftSlope, float topSlope, float bottomSlope,
                                  float near, float far) :
            Origin(origin),
            Rotation(rot),
            RightSlope(rightSlope),
            LeftSlope(leftSlope),
            TopSlope(topSlope),
            BottomSlope(bottomSlope),
            Near(near),
            Far(far) {}

        // ==========
        // Inquirers
        // ==========


    };
}
