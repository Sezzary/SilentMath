#pragma once

namespace Silent::Math
{
    class AxisAngle;
    class Matrix;
    class Vector3i;

    class Vector3 : public glm::vec3
    {
    public:
        // ==========
        // Constants
        // ==========

        static constexpr int AXIS_COUNT = 3;

        // ========
        // Presets
        // ========

        static const Vector3 Zero;
        static const Vector3 One;
        static const Vector3 UnitX;
        static const Vector3 UnitY;
        static const Vector3 UnitZ;

        // =============
        // Constructors
        // =============

        /** @brief Creates a default instance. */
        constexpr Vector3() = default;

        /** @brief Creates an instance from XYZ componets.
         *
         * @param x X component.
         * @param y Y component.
         * @param z Z component.
         */
        constexpr Vector3(float x) : glm::vec3(x) {}

        /** @brief Creates an instance from a `glm::vec3`.
         *
         * @param vec `float`-based GLM XYZ vector.
         */
        constexpr Vector3(float x, float y, float z) : glm::vec3(x, y, z) {}

        /** @brief Creates an instance with all components set to the same value.
         *
         * @param x Value to set for all components.
         */
        constexpr Vector3(const glm::vec3& vec) : glm::vec3(vec) {}

        // ==========
        // Utilities
        // ==========

        /** @brief Computes the magnitude. */
        float Length() const;

        /** @brief Computes the squared magnitude. */
        float LengthSquared() const;

        /** @brief Computes the distance between two vectors.
         *
         * @param from Start vector.
         * @param to End vector.
         * @return Distance between vectors.
         */
        static float Distance(const Vector3& from, const Vector3& to);

        /** @brief Computes the squared distance between two vectors.
         *
         * @param from Start vector.
         * @param to End vector.
         * @return Squared distance between vectors.
         */
        static float DistanceSquared(const Vector3& from, const Vector3& to);

        /** @brief Computes the dot product of two vectors.
         *
         * @param vec0 First vector.
         * @param vec1 Second vector.
         * @return Dot product.
         */
        static float Dot(const Vector3& vec0, const Vector3& vec1);

        /** @brief Computes the cross product of two vectors.
         *
         * @param vec0 First vector.
         * @param vec1 Second vector.
         * @return Cross product.
         */
        static Vector3 Cross(const Vector3& vec0, const Vector3& vec1);

        /** @brief Computes the minimum of two vectors.
         *
         * @param vec0 First vector.
         * @param vec1 Second vector.
         * @return Minimum.
         */
        static Vector3 Min(const Vector3& vec0, const Vector3& vec1);

        /** @brief Computes the minimum of the vector and a second vector.
         *
         * @param vec Second vector.
         */
        void Min(const Vector3& vec);

        /** @brief Computes the maximum of two vectors.
         *
         * @param vec0 First vector.
         * @param vec1 Second vector.
         * @return Maximum.
         */
        static Vector3 Max(const Vector3& vec0, const Vector3& vec1);

        /** @brief Computes the maximum of the vector and a second vector.
         *
         * @param vec Second vector.
         */
        void Max(const Vector3& vec);

        /** @brief Clamps a vector to within the range `[min, max]`.
         *
         * @param vec Vector to clamp.
         * @param min Minimum range (inclusive).
         * @param max Maximum range (inclusive).
         * @return Clamped vector.
         */
        static Vector3 Clamp(const Vector3& vec, const Vector3& min, const Vector3& max);

        /** @brief Clamps to within the range `[min, max]`.
         *
         * @param min Minimum range (inclusive).
         * @param max Maximum range (inclusive).
         */
        void Clamp(const Vector3& min, const Vector3& max);

        /** @brief Normalizes the vector.
         *
         * @param vec Vector to normalize.
         * @return Normalized vector.
         */
        static Vector3 Normalize(const Vector3& vec);

        /** @brief Normalized the vector. */
        void Normalize();

        /** @brief Linearly interpolates between two vectors.
         *
         * @param from Start vector.
         * @param to End vector.
         * @param alpha Interpolation alpha.
         * @return Interpolated vector.
         */
        static Vector3 Lerp(const Vector3& from, const Vector3& to, float alpha);

        /** @brief Linearly interpolates with another vector.
         *
         * @param to End vector.
         * @param alpha Interpolation alpha.
         */
        void Lerp(const Vector3& to, float alpha);

        /** @brief Transforms a vector using a transformation matrix.
         *
         * @param vec Vector to transform.
         * @param mat Transformation matrix.
         * @return Transformed vector.
         */
        static Vector3 Transform(const Vector3& vec, const Matrix& mat);

        /** @brief Transforms using a transformation matrix.
         *
         * @param mat Transformation matrix.
         */
        void Transform(const Matrix& mat);

        /** @brief Translates a vector in a given direction by a distance.
         *
         * @param vec Vector to translate.
         * @param dir Translation direction.
         * @param dist Translation distance.
         * @return Translated vector.
         */
        static Vector3 Translate(const Vector3& vec, const Vector3& dir, float dist);

        /** @brief Translates in a given direction by a distance.
         *
         * @param dir Translation direction.
         * @param dist Translation distance.
         */
        void Translate(const Vector3& dir, float dist);

        /** @brief Translates a vector by a given relative offset according to the angle from a reference down axis.
         *
         * @param vec Vector to translate.
         * @param headingRad Heading angle in radians.
         * @param relOffset Relative offset.
         * @param axis Normalized down axis.
         * @return Translated vector.
         */
        static Vector3 Translate(const Vector3& vec, float headingRad, const Vector3& relOffset, const Vector3& axis = -Vector3::UnitY);

        /** @brief Translates by a given relative offset according to the angle from a reference down axis.
         *
         * @param headingRad Heading angle in radians.
         * @param relOffset Relative offset.
         * @param axis Normalized down axis.
         */
        void Translate(float headingRad, const Vector3& relOffset, const Vector3& axis = -Vector3::UnitY);

        /** @brief Compares two vectors with a precision epsilon.
         *
         * @param vec0 First vector.
         * @param vec1 Second vector.
         * @param epsilon Precision threshold.
         * @return `true` if the vectors are within the `epsilon` threshold of each other, `false` otherwise.
         */
        static bool Compare(const Vector3& vec0, const Vector3& vec1, float epsilon = EPSILON);

        // ===========
        // Converters
        // ===========

        /** @brief Converts to a rotation matrix.
         *
         * @return Rotation matrix.
         */
        Matrix ToTranslationMatrix() const;

        /** @brief Converts to a scale matrix.
         *
         * @return Scale matrix.
         */
        Matrix ToScaleMatrix() const;

        /** @brief Converts to a `Vector3i` with an optional fixed-point Q format shift.
         *
         * @param shift Q shift.
         * @return `int`-based XYZ vector.
         */
        Vector3i ToVector3i(int shift = NO_VALUE) const;

        /** @brief Converts to the read-only `glm::vec3` base object.
         *
         * @return Ready-only `glm::vec3` base object.
         */
        const glm::vec3& ToGlmVec3() const;

        /** @brief Converts to the writable `glm::vec3` base object.
         *
         * @return Writable `glm::vec3` base object.
         */
        glm::vec3& ToGlmVec3();

        // ==========
        // Operators
        // ==========

        bool     operator==(const Vector3& vec) const;
        bool     operator!=(const Vector3& vec) const;
        Vector3& operator=(const Vector3& vec) = default;
        Vector3& operator+=(const Vector3& vec);
        Vector3& operator-=(const Vector3& vec);
        Vector3& operator*=(const Vector3& vec);
        Vector3& operator*=(float scalar);
        Vector3& operator/=(const Vector3& vec);
        Vector3& operator/=(float scalar);
        Vector3  operator+(const Vector3& vec) const;
        Vector3  operator-(const Vector3& vec) const;
        Vector3  operator*(const Vector3& vec) const;
        Vector3  operator*(float scalar) const;
        Vector3  operator/(const Vector3& vec) const;
        Vector3  operator/(float scalar) const;
        Vector3  operator-() const;
    };
}

namespace std
{
    template <>
    struct hash<Silent::Math::Vector3>
    {
        size_t operator()(const Silent::Math::Vector3& vec) const
        {
            size_t seed = 0;
            seed       ^= ((hash<float>()(vec.x) + Silent::Math::GOLDEN_RATIO) + (seed << 6)) + (seed >> 2);
            seed       ^= ((hash<float>()(vec.y) + Silent::Math::GOLDEN_RATIO) + (seed << 6)) + (seed >> 2);
            seed       ^= ((hash<float>()(vec.z) + Silent::Math::GOLDEN_RATIO) + (seed << 6)) + (seed >> 2);
            return seed;
        }
    };
}
