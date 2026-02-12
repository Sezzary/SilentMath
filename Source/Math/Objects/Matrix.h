#pragma once

namespace Silent::Math
{
    class AxisAngle;
    class EulerAngles;
    class Quaternion;
    class Vector3;

    /** @brief 4x4 matrix. */
    class Matrix : public glm::mat4
    {
    public:
        // ========
        // Presets
        // ========

        static const Matrix Identity;

        // =============
        // Constructors
        // =============

        /** @brief Creates a default identity instance. */
        constexpr Matrix() : glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                                       0.0f, 1.0f, 0.0f, 0.0f,
                                       0.0f, 0.0f, 1.0f, 0.0f,
                                       0.0f, 0.0f, 0.0f, 1.0f) {}

        /** @brief Creates an instance from a `glm::mat4`.
         *
         * @param mat GLM 4x4 matrix.
         */
        constexpr Matrix(const glm::mat4& mat) : glm::mat4(mat) {}

        /** @brief Creates an instance with all components set to the same value.
         *
         * @param m Value to set for all components.
         */
        constexpr Matrix(float m) : glm::mat4(m) {}

        /** @brief Creates a 4x4 instance.
         *
         * @param m00 First row, first column component.
         * @param m01 First row, second column component.
         * @param m02 First row, third column component.
         * @param m03 First row, fourth column component.
         * @param m10 Second row, first column component.
         * @param m11 Second row, second column component.
         * @param m12 Second row, third column component.
         * @param m13 Second row, fourth column component.
         * @param m20 Third row, first column component.
         * @param m21 Third row, second column component.
         * @param m22 Third row, third column component.
         * @param m23 Third row, fourth column component.
         * @param m30 Fourth row, first column component.
         * @param m31 Fourth row, second column component.
         * @param m32 Fourth row, third column component.
         * @param m33 Fourth row, fourth column component.
         */
        constexpr Matrix(float m00, float m01, float m02, float m03,
                         float m10, float m11, float m12, float m13,
                         float m20, float m21, float m22, float m23,
                         float m30, float m31, float m32, float m33) : glm::mat4(m00, m01, m02, m03,
                                                                                 m10, m11, m12, m13,
                                                                                 m20, m21, m22, m23,
                                                                                 m30, m31, m32, m33) {}

        /** @brief Creates a 3x3 instance.
         *
         * @param m00 First row, first column component.
         * @param m01 First row, second column component.
         * @param m02 First row, third column component.
         * @param m10 Second row, first column component.
         * @param m11 Second row, second column component.
         * @param m12 Second row, third column component.
         * @param m20 Third row, first column component.
         * @param m21 Third row, second column component.
         * @param m22 Third row, third column component.
         */
        constexpr Matrix(float m00, float m01, float m02,
                         float m10, float m11, float m12,
                         float m20, float m21, float m22) : glm::mat4(m00,  m01,  m02,  0.0f,
                                                                      m10,  m11,  m12,  0.0f,
                                                                      m20,  m21,  m22,  0.0f,
                                                                      0.0f, 0.0f, 0.0f, 1.0f) {}

        /** @brief Creates a translation instance.
         *
         * @param translation Translation offset.
         */
        static Matrix CreateTranslation(const Vector3& translation);

        /** @brief Creates an X-axis rotation instance.
         *
         * @param rad Rotation in radians.
         */
        static Matrix CreateRotationX(float rad);

        /** @brief Creates a Y-axis rotation instance.
         *
         * @param rad Y rotation in radians.
         */
        static Matrix CreateRotationY(float rad);

        /** @brief Creates a Z-axis rotation instance.
         *
         * @param rad Z rotation in radians.
         */
        static Matrix CreateRotationZ(float rad);

        /** @brief Creates a scale instance.
         *
         * @param scale Scale.
         */
        static Matrix CreateScale(const Vector3& scale);

        /** @brief Creates an orthographic projection instance.
         *
         * @param left Minimum X coordinate of the view volume.
         * @param right Maximum X coordinate of the view volume.
         * @param bottom Minimum Y coordinate of the view volume.
         * @param top Maximum Y coordinate of the view volume.
         */
        static Matrix CreateOrthographic(float left, float right, float bottom, float top);

        /** @brief Creates an orthographic projection instance.
         *
         * @param left Minimum X coordinate of the view volume.
         * @param right Maximum X coordinate of the view volume.
         * @param bottom Minimum Y coordinate of the view volume.
         * @param top Maximum Y coordinate of the view volume.
         * @param nearPlane Frustum near plane.
         * @param farPlane Frustum far plane.
         */
        static Matrix CreateOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);

        /** @brief Creates a perspective projection instance.
         *
         * @param fov Field-of-view angle in radians.
         * @param aspect 
         * @param nearPlane Frustum near plane.
         * @param farPlane Frustum far plane.
         */
        static Matrix CreatePerspective(float fov, float aspect, float nearPlane, float farPlane);

        /** @brief Creates a view look-at instance.
         *
         * @param pos View position.
         * @param target Look-at position.
         * @param up Up axis.
         */
        static Matrix CreateLookAt(const Vector3& pos, const Vector3& target, const Vector3& up);

        // ==========
        // Utilities
        // ==========

        /** @brief Inverts a matrix. */
        static Matrix Inverse(const Matrix& mat);

        /** @brief Inverts the matrix. */
        void Inverse();

        /** @brief Linearly interpolates between two matrices.
         *
         * @param from Start matrix.
         * @param to End matrix.
         * @param alpha Interpolation alpha.
         * @return Linearly interpolated matrix.
         */
        static Matrix Lerp(const Matrix& from, const Matrix& to, float alpha);

        /** @brief Linearly interpolates between the matrix and another matrix.
         *
         * @param to End matrix.
         * @param alpha Interpolation alpha.
         */
        void Lerp(const Matrix& to, float alpha);

        /** @brief Translates a matrix by a given offset.
         *
         * @param mat Matrix to translate.
         * @param translation Translation offset.
         * @return Translated matrix.
         */
        static Matrix Translate(const Matrix& mat, const Vector3& translation);

        /** @brief Translates the matrix by a given offset.
         *
         * @param translation Translation offset.
         */
        void Translate(const Vector3& translation);

        static Matrix Rotate(const Matrix& mat, float rad, const Vector3& axis);

        void Rotate(float rad, const Vector3& axis);

        /** @brief Scales a matrix by a given scale.
         *
         * @param mat Matrix to scale.
         * @param scale XYZ scale factor.
         * @return Scaled matrix.
         */
        static Matrix Scale(const Matrix& mat, const Vector3& scale);

        /** @brief Scales the matrix by a given scale.
         *
         * @param scale XYZ scale factor.
         */
        void Scale(const Vector3& scale);

        // @todo
        // LookAt
        // Transpose
        // Determinant

        // ===========
        // Converters
        // ===========

        /** @brief Extracts the translation offset as a vector.
         *
         * @return Translation offset.
         */
        Vector3 ToTranslation() const;

        /** @brief Extracts the rotation as a direction vector.
         *
         * @return Direction vector corresponding to the rotation.
         */
        Vector3 ToDirection() const;

        /** @brief Extracts the rotation as Euler angles.
         * 
         * @return Euler angles rotation.
         */
        EulerAngles ToEulerAngles() const;

        /** @brief Extracts the rotation as a quaternion.
         * 
         * @return Quaternion rotation.
         */
        Quaternion ToQuaternion() const;

        /** @brief Extracts the rotation as an axis-angle.
         * 
         * @return Axis-angle rotation.
         */
        AxisAngle ToAxisAngle() const;

        /** @brief Extracts the scale as an XYZ scale factor.
         *
         * @return XYZ scale factor.
         */
        Vector3 ToScale() const;

        /** @brief Downcasts to a read-only `glm::mat4`.
         *
         * @return Read-only `glm::mat4`.
         */
        const glm::mat4& ToGlmMat4() const;

        /** @brief Downcasts to a writable `glm::mat4`.
         *
         * @return Writable `glm::mat4`.
         */
        glm::mat4& ToGlmMat4();

        // ==========
        // Operators
        // ==========

        bool    operator==(const Matrix& mat) const;
        bool    operator!=(const Matrix& mat) const;
        Matrix& operator=(const Matrix& mat) = default;
        Matrix& operator+=(const Matrix& mat);
        Matrix& operator-=(const Matrix& mat);
        Matrix& operator*=(const Matrix& mat);
        Matrix& operator*=(float scalar);
        Matrix& operator/=(const Matrix& mat);
        Matrix& operator/=(float scalar);
    };
}
