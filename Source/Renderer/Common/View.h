#pragma once

namespace Silent::Renderer
{
    /** @brief 3D render view. */
    class View
    {
    public:
        // =======
        // Fields
        // =======

        Vector3         Position  = Vector3::Zero;
        Vector3         Direction = Vector3::UnitZ;
        Vector3         Up        = Vector3::UnitY;
        Vector2i        Size      = Vector2i::Zero;  
        float           Fov       = DEG_TO_RAD(45.0f);
        BoundingFrustum Frustum   = BoundingFrustum{};

        // =============
        // Constructors
        // =============

        /** @brief Creates a default uninitialized instance. */
        View() = default;

        /** @brief Creates an instance from a position and size.
         *
         * @param pos World position.
         * @param size Screen size.
         */
        View(const Vector3& pos, const Vector2i& size);

        // ==========
        // Utilities
        // ==========

        /** @brief Computes the combined view and projection matrix.
         *
         * @param fov Field of view in radians.
         * @param aspect Aspect ratio.
         * @param nearPlane Near plane.
         * @param farPlane Far plane.
         */
        Matrix GetMatrix(float fov, float aspect, float nearPlane, float farPlane);

        void Move();
    };
}
