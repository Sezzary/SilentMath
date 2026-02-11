#pragma once

namespace Silent::Math
{
    class Matrix;
    class Vector2;

    class Vector2i : public glm::ivec2
    {
    public:
        // ==========
        // Constants
        // ==========

        static constexpr int AXIS_COUNT = 2;

        // ========
        // Presets
        // ========

        static const Vector2i Zero;
        static const Vector2i One;
        static const Vector2i UnitX;
        static const Vector2i UnitY;

        // =============
        // Constructors
        // =============

        /** @brief Creates a default instance. */
        constexpr Vector2i() = default;

        /** @brief Creates an instance with all components set to the same value.
         *
         * @param x Value to set for all components.
         */
        constexpr Vector2i(int x) : glm::ivec2(x) {}

        /** @brief Creates an instance from XY componets.
         *
         * @param x X component.
         * @param y Y component.
         */
        constexpr Vector2i(int x, int y) : glm::ivec2(x, y) {}

        /** @brief Creates an instance from a `glm::ivec2`.
         *
         * @param vec `int`-based GLM XY vector.
         */
        constexpr Vector2i(const glm::ivec2& vec) : glm::ivec2(vec) {}

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
        static float Distance(const Vector2i& from, const Vector2i& to);

        /** @brief Computes the squared distance between two vectors.
         *
         * @param from Start vector.
         * @param to End vector.
         * @return Squared distance between vectors.
         */
        static float DistanceSquared(const Vector2i& from, const Vector2i& to);

        /** @brief Computes the minimum of two vectors.
         *
         * @param vec0 First vector.
         * @param vec1 Second vector.
         * @return Minimum.
         */
        static Vector2i Min(const Vector2i& vec0, const Vector2i& vec1);

        /** @brief Computes the minimum of the vector and a second vector.
         *
         * @param vec Second vector.
         */
        void Min(const Vector2i& vec);

        /** @brief Computes the maximum of two vectors.
         *
         * @param vec0 First vector.
         * @param vec1 Second vector.
         * @return Maximum.
         */
        static Vector2i Max(const Vector2i& vec0, const Vector2i& vec1);

        /** @brief Computes the maximum of the vector and a second vector.
         *
         * @param vec Second vector.
         */
        void Max(const Vector2i& vec);

        /** @brief Clamps a vector to within the range `[min, max]`.
         *
         * @param vec Vector to clamp.
         * @param min Minimum range (inclusive).
         * @param max Maximum range (inclusive).
         * @return Clamped vector.
         */
        static Vector2i Clamp(const Vector2i& vec, const Vector2i& min, const Vector2i& max);

        /** @brief Clamps to within the range `[min, max]`.
         *
         * @param min Minimum range (inclusive).
         * @param max Maximum range (inclusive).
         */
        void Clamp(const Vector2i& min, const Vector2i& max);

        /** @brief Linearly interpolates between two vectors.
         *
         * @param from Start vector.
         * @param to End vector.
         * @param alpha Interpolation alpha.
         * @return Interpolated vector.
         */
        static Vector2i Lerp(const Vector2i& from, const Vector2i& to, float alpha);

        /** @brief Linearly interpolates with another vector.
         *
         * @param to End vector.
         * @param alpha Interpolation alpha.
         */
        void Lerp(const Vector2i& to, float alpha);

        /** @brief Transforms a vector using a transformation matrix.
         *
         * @param vec Vector to transform.
         * @param mat Transformation matrix.
         * @return Transformed vector.
         */
        static Vector2i Transform(const Vector2i& vec, const Matrix& mat);

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
        static Vector2i Translate(const Vector2i& vec, const Vector2& dir, float dist);

        /** @brief Translates in a given direction by a distance.
         *
         * @param dir Translation direction.
         * @param dist Translation distance.
         */
        void Translate(const Vector2& dir, float dist);

        // ===========
        // Converters
        // ===========

        /** @brief Converts to a `Vector2` with an optional fixed-point Q format shift.
         *
         * @param shift Q shift.
         * @return `float`-based XY vector.
         */
        Vector2 ToVector2(int shift = NO_VALUE) const;

        /** @brief Converts to the read-only `glm::ivec2` base object.
         *
         * @return Ready-only `glm::ivec2` base object.
         */
        const glm::ivec2& ToGlmVec2i() const;

        /** @brief Converts to the writable `glm::ivec2` base object.
         *
         * @return Writable `glm::ivec2` base object.
         */
        glm::ivec2& ToGlmVec2i();

        // ==========
        // Operators
        // ==========

        bool      operator==(const Vector2i& vec) const;
        bool      operator!=(const Vector2i& vec) const;
        Vector2i& operator=(const Vector2i& vec) = default;
        Vector2i& operator+=(const Vector2i& vec);
        Vector2i& operator-=(const Vector2i& vec);
        Vector2i& operator*=(const Vector2i& vec);
        Vector2i& operator*=(float scalar);
        Vector2i& operator/=(const Vector2i& vec);
        Vector2i& operator/=(float scalar);
        Vector2i  operator+(const Vector2i& vec) const;
        Vector2i  operator-(const Vector2i& vec) const;
        Vector2i  operator*(const Vector2i& vec) const;
        Vector2i  operator*(float scalar) const;
        Vector2i  operator/(const Vector2i& vec) const;
        Vector2i  operator/(float scalar) const;
        Vector2i  operator-() const;
    };
}

namespace std
{
    template <>
    struct hash<Silent::Math::Vector2i>
    {
        size_t operator()(const Silent::Math::Vector2i& vec) const
        {
            size_t seed = 0;
            seed       ^= ((hash<int>()(vec.x) + Silent::Math::GOLDEN_RATIO) + (seed << 6)) + (seed >> 2);
            seed       ^= ((hash<int>()(vec.y) + Silent::Math::GOLDEN_RATIO) + (seed << 6)) + (seed >> 2);
            return seed;
        }
    };
}
