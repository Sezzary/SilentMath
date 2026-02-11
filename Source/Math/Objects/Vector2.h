#pragma once

namespace Silent::Math
{
    class Matrix;
    class Vector2i;

    class Vector2 : public glm::vec2
    {
    public:
        // ==========
        // Constants
        // ==========

        static constexpr int AXIS_COUNT = 2;

        // ========
        // Presets
        // ========

        static const Vector2 Zero;
        static const Vector2 One;
        static const Vector2 UnitX;
        static const Vector2 UnitY;

        // =============
        // Constructors
        // =============

        /** @brief Creates a default instance. */
        constexpr Vector2() = default;

        /** @brief Creates an instance with all components set to the same value.
         *
         * @param x Value to set for all components.
         */
        constexpr Vector2(float x) : glm::vec2(x) {}

        /** @brief Creates an instance from XY componets.
         *
         * @param x X component.
         * @param y Y component.
         */
        constexpr Vector2(float x, float y) : glm::vec2(x, y) {}

        /** @brief Creates an instance from a `glm::vec2`.
         *
         * @param vec `float`-based GLM XY vector.
         */
        constexpr Vector2(const glm::vec2& vec) : glm::vec2(vec) {}

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
        static float Distance(const Vector2& from, const Vector2& to);

        /** @brief Computes the squared distance between two vectors.
         *
         * @param from Start vector.
         * @param to End vector.
         * @return Squared distance between vectors.
         */
        static float DistanceSquared(const Vector2& from, const Vector2& to);

        /** @brief Computes the dot product of two vectors.
         *
         * @param vec0 First vector.
         * @param vec1 Second vector.
         * @return Dot product.
         */
        static float Dot(const Vector2& vec0, const Vector2& vec1);

        /** @brief Computes the cross product of two vectors.
         *
         * @param vec0 First vector.
         * @param vec1 Second vector.
         * @return Cross product.
         */
        static float Cross(const Vector2& vec0, const Vector2& vec1);

        /** @brief Computes the minimum of two vectors.
         *
         * @param vec0 First vector.
         * @param vec1 Second vector.
         * @return Minimum.
         */
        static Vector2 Min(const Vector2& vec0, const Vector2& vec1);

        /** @brief Computes the minimum of the vector and a second vector.
         *
         * @param vec Second vector.
         */
        void Min(const Vector2& vec);
    
        /** @brief Computes the maximum of two vectors.
         *
         * @param vec0 First vector.
         * @param vec1 Second vector.
         */
        static Vector2 Max(const Vector2& vec0, const Vector2& vec1);
    
        /** @brief Computes the maximum of the vector and a second vector.
         *
         * @param vec Second vector.
         */
        void Max(const Vector2& vec);

        /** @brief Clamps a vector to within the range `[min, max]`.
         *
         * @param vec Vector to clamp.
         * @param min Minimum range (inclusive).
         * @param max Maximum range (inclusive).
         * @return Clamped vector.
         */
        static Vector2 Clamp(const Vector2& vec, const Vector2& min, const Vector2& max);

        /** @brief Clamps to within the range `[min, max]`.
         *
         * @param min Minimum range (inclusive).
         * @param max Maximum range (inclusive).
         */
        void Clamp(const Vector2& min, const Vector2& max);

        /** @brief Normalizes the vector.
         *
         * @param vec Vector to normalize.
         * @return Normalized vector.
         */
        static Vector2 Normalize(const Vector2& vec);

        /** @brief Normalized the vector. */
        void Normalize();

        /** @brief Linearly interpolates between two vectors.
         *
         * @param from Start vector.
         * @param to End vector.
         * @param alpha Interpolation alpha.
         * @return Interpolated vector.
         */
        static Vector2 Lerp(const Vector2& from, const Vector2& to, float alpha);

        /** @brief Linearly interpolates with another vector.
         *
         * @param to End vector.
         * @param alpha Interpolation alpha.
         */
        void Lerp(const Vector2& to, float alpha);

        /** @brief Transforms a vector using a transformation matrix.
         *
         * @param vec Vector to transform.
         * @param mat Transformation matrix.
         * @return Transformed vector.
         */
        static Vector2 Transform(const Vector2& vec, const Matrix& at);

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
        static Vector2 Translate(const Vector2& vec, const Vector2& dir, float dist);

        /** @brief Translates in a given direction by a distance.
         *
         * @param dir Translation direction.
         * @param dist Translation distance.
         */
        void Translate(const Vector2& dir, float dist);

        /** @brief Compares two vectors with a precision epsilon.
         *
         * @param vec0 First vector.
         * @param vec1 Second vector.
         * @param epsilon Precision threshold.
         * @return `true` if the vectors are within the `epsilon` threshold of each other, `false` otherwise.
         */
        static bool Compare(const Vector2& vec0, const Vector2& vec1, float epsilon = EPSILON);

        // ===========
        // Converters
        // ===========

        /** @brief Converts to a `Vector2i` with an optional fixed-point Q format shift.
         *
         * @param shift Q shift.
         * @return `int`-based XY vector.
         */
        Vector2i ToVector2i(int shift = NO_VALUE) const;

        /** @brief Converts to the read-only `glm::vec2` base object.
         *
         * @return Ready-only `glm::vec2` base object.
         */
        const glm::vec2& ToGlmVec2() const;

        /** @brief Converts to the writable `glm::vec2` base object.
         *
         * @return Writable `glm::vec2` base object.
         */
        glm::vec2& ToGlmVec2();

        // ==========
        // Operators
        // ==========

        bool     operator==(const Vector2& vec) const;
        bool     operator!=(const Vector2& vec) const;
        Vector2& operator=(const Vector2& vec) = default;
        Vector2& operator+=(const Vector2& vec);
        Vector2& operator-=(const Vector2& vec);
        Vector2& operator*=(const Vector2& vec);
        Vector2& operator*=(float scalar);
        Vector2& operator/=(const Vector2& vec);
        Vector2& operator/=(float scalar);
        Vector2  operator+(const Vector2& vec) const;
        Vector2  operator-(const Vector2& vec) const;
        Vector2  operator*(const Vector2& vec) const;
        Vector2  operator*(float scalar) const;
        Vector2  operator/(const Vector2& vec) const;
        Vector2  operator/(float scalar) const;
        Vector2  operator-() const;
    };
}

namespace std
{
    template <>
    struct hash<Silent::Math::Vector2>
    {
        size_t operator()(const Silent::Math::Vector2& vec) const
        {
            size_t seed = 0;
            seed       ^= ((hash<float>()(vec.x) + Silent::Math::GOLDEN_RATIO) + (seed << 6)) + (seed >> 2);
            seed       ^= ((hash<float>()(vec.y) + Silent::Math::GOLDEN_RATIO) + (seed << 6)) + (seed >> 2);
            return seed;
        }
    };
}
