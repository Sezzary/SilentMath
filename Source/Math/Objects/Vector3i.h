#pragma once

#include "Math/Constants.h"

namespace Silent::Math
{
    class Matrix;
    class Vector3;

    class Vector3i : public glm::ivec3
    {
    public:
        // ==========
        // Constants
        // ==========

        static constexpr int AXIS_COUNT = 3;

        // ========
        // Presets
        // ========

        static const Vector3i Zero;
        static const Vector3i One;
        static const Vector3i UnitX;
        static const Vector3i UnitY;
        static const Vector3i UnitZ;

        // =============
        // Constructors
        // =============

        /** @brief Creates a default instance. */
        constexpr Vector3i() = default;

        /** @brief Creates an instance with all components set to the same value.
         *
         * @param x Value to set for all components.
         */
        constexpr Vector3i(int x) : glm::ivec3(x) {}

        /** @brief Creates an instance from XYZ componets.
         *
         * @param x X component.
         * @param y Y component.
         * @param z Z component.
         */
        constexpr Vector3i(int x, int y, int z) : glm::ivec3(x, y, z) {}

        /** @brief Creates an instance from a `glm::ivec3`.
         *
         * @param vec `int`-based GLM XYZ vector.
         */
        constexpr Vector3i(const glm::ivec3& vec) : glm::ivec3(vec) {}

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
        static float Distance(const Vector3i& from, const Vector3i& to);

        /** @brief Computes the squared distance between two vectors.
         *
         * @param from Start vector.
         * @param to End vector.
         * @return Squared distance between vectors.
         */
        static float DistanceSquared(const Vector3i& from, const Vector3i& to);

        /** @brief Computes the minimum of two vectors.
         *
         * @param vec0 First vector.
         * @param vec1 Second vector.
         * @return Minimum.
         */
        static Vector3i Min(const Vector3i& vec0, const Vector3i& vec1);

        /** @brief Computes the minimum of the vector and a second vector.
         *
         * @param vec Second vector.
         */
        void Min(const Vector3i& vec);

        /** @brief Computes the maximum of two vectors.
         *
         * @param vec0 First vector.
         * @param vec1 Second vector.
         * @return Maximum.
         */
        static Vector3i Max(const Vector3i& vec0, const Vector3i& vec1);

        /** @brief Computes the maximum of the vector and a second vector.
         *
         * @param vec Second vector.
         */
        void Max(const Vector3i& vec);

        /** @brief Clamps a vector to within the range `[min, max]`.
         *
         * @param vec Vector to clamp.
         * @param min Minimum range (inclusive).
         * @param max Maximum range (inclusive).
         * @return Clamped vector.
         */
        static Vector3i Clamp(const Vector3i& vec, const Vector3i& min, const Vector3i& max);

        /** @brief Clamps to within the range `[min, max]`.
         *
         * @param min Minimum range (inclusive).
         * @param max Maximum range (inclusive).
         */
        void Clamp(const Vector3i& min, const Vector3i& max);

        /** @brief Linearly interpolates between two vectors.
         *
         * @param from Start vector.
         * @param to End vector.
         * @param alpha Interpolation alpha.
         * @return Interpolated vector.
         */
        static Vector3i Lerp(const Vector3i& from, const Vector3i& to, float alpha);

        /** @brief Linearly interpolates with another vector.
         *
         * @param to End vector.
         * @param alpha Interpolation alpha.
         */
        void Lerp(const Vector3i& to, float alpha);

        /** @brief Transforms a vector using a transformation matrix.
         *
         * @param vec Vector to transform.
         * @param mat Transformation matrix.
         * @return Transformed vector.
         */
        static Vector3i Transform(const Vector3i& vec, const Matrix& mat);

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
        static Vector3i Translate(const Vector3i& vec, const Vector3& dir, float dist);

        /** @brief Translates in a given direction by a distance.
         *
         * @param dir Translation direction.
         * @param dist Translation distance.
         */
        void Translate(const Vector3& dir, float dist);

        // ===========
        // Converters
        // ===========

        /** @brief Converts to a `Vector3` with an optional Q shift if the components are in a fixed-point format.
         *
         * @param shift Fixed-point Q shift.
         * @return `Vector3` conversion result.
         */
        Vector3 ToVector3(int shift = NO_VALUE) const;

        /** @brief Converts to the read-only `glm::ivec3` base object.
         *
         * @return Ready-only `glm::ivec3` base object.
         */
        const glm::ivec3& ToGlmVec3i() const;

        /** @brief Converts to the writable `glm::ivec3` base object.
         *
         * @return Writable `glm::ivec3` base object.
         */
        glm::ivec3& ToGlmVec3i();

        // ==========
        // Operators
        // ==========

        bool      operator==(const Vector3i& vec) const;
        bool      operator!=(const Vector3i& vec) const;
        Vector3i& operator=(const Vector3i& vec) = default;
        Vector3i& operator+=(const Vector3i& vec);
        Vector3i& operator-=(const Vector3i& vec);
        Vector3i& operator*=(const Vector3i& vec);
        Vector3i& operator*=(float scalar);
        Vector3i& operator/=(const Vector3i& vec);
        Vector3i& operator/=(float scalar);
        Vector3i  operator+(const Vector3i& vec) const;
        Vector3i  operator-(const Vector3i& vec) const;
        Vector3i  operator*(const Vector3i& vec) const;
        Vector3i  operator*(float scalar) const;
        Vector3i  operator/(const Vector3i& vec) const;
        Vector3i  operator/(float scalar) const;
        Vector3i  operator-() const;
    };
}

namespace std
{
    template <>
    struct hash<Silent::Math::Vector3i>
    {
        size_t operator()(const Silent::Math::Vector3i& vec) const
        {
            size_t seed = 0;
            seed       ^= ((hash<int>()(vec.x) + Silent::Math::GOLDEN_RATIO) + (seed << 6)) + (seed >> 2);
            seed       ^= ((hash<int>()(vec.y) + Silent::Math::GOLDEN_RATIO) + (seed << 6)) + (seed >> 2);
            seed       ^= ((hash<int>()(vec.z) + Silent::Math::GOLDEN_RATIO) + (seed << 6)) + (seed >> 2);
            return seed;
        }
    };
}
