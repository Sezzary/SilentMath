#pragma once

#include "Math/Constants.h"

namespace Silent::Math
{
    class Matrix;

    class Vector2 : public glm::vec2
    {
    public:
        // Constants

        static constexpr uint AXIS_COUNT = 3;

        // Presets

        static const Vector2 Zero;
        static const Vector2 One;
        static const Vector2 UnitX;
        static const Vector2 UnitY;

        // Constructors

        constexpr Vector2() = default;
        constexpr Vector2(const glm::vec2& vec) : glm::vec2(vec) {}
        constexpr Vector2(float x)                               { this->x = x; this->y = x; }
        constexpr Vector2(float x, float y)                      { this->x = x; this->y = y; }

        // Utilities

        static bool  Compare(const Vector2& vec0, const Vector2& vec1, float epsilon = EPSILON);
        static float Length(const Vector2& vec);
        float        Length() const;
        static float LengthSquared(const Vector2& vec);
        float        LengthSquared() const;
        static float Distance(const Vector2& from, const Vector2& to);
        float        Distance(const Vector2& to) const;
        static float DistanceSquared(const Vector2& from, const Vector2& to);
        float        DistanceSquared(const Vector2& to) const;
        static float Dot(const Vector2& vec0, const Vector2& vec1);
        float        Dot(const Vector2& vec) const;
        static float Cross(const Vector2& vec0, const Vector2& vec1);
        Vector2      Cross(const Vector2& vec) const;

        static Vector2 Normalize(const Vector2& vec);
        void           Normalize();
        static Vector2 Lerp(const Vector2& from, const Vector2& to, float alpha);
        void           Lerp(const Vector2& to, float alpha);
        static Vector2 Smoothstep(const Vector2& from, const Vector2& to, float alpha);
        void           Smoothstep(const Vector2& to, float alpha);
        static Vector2 Transform(const Vector2& vec, const Matrix& mat);
        void           Transform(const Matrix& mat);
        static Vector2 Translate(const Vector2& vec, const Vector2& dir, float dist);
        void           Translate(const Vector2& dir, float dist);

        // Converters

        const glm::vec2& ToGlmVec2() const;
        glm::vec2&       ToGlmVec2();

        // Operators

        bool     operator==(const Vector2& vec) const;
        bool     operator!=(const Vector2& vec) const;
        Vector2& operator=(const Vector2& vec) = default;
        Vector2& operator+=(const Vector2& vec);
        Vector2& operator-=(const Vector2& vec);
        Vector2& operator*=(const Vector2& vec);
        Vector2& operator*=(float scalar);
        Vector2& operator/=(float scalar);
        Vector2  operator+(const Vector2& vec) const;
        Vector2  operator-(const Vector2& vec) const;
        Vector2  operator*(const Vector2& vec) const;
        Vector2  operator*(float scalar) const;
        Vector2  operator/(float scalar) const;
        Vector2  operator-() const;
    };
}
