#pragma once

#include "Math/Constants.h"

namespace Silent::Math
{
    class Matrix;
    class Vector3;

    class Vector3i : public glm::ivec3
    {
    public:
        // Constants

        static constexpr uint AXIS_COUNT = 3;

        // Presets

        static const Vector3i Zero;
        static const Vector3i One;
        static const Vector3i UnitX;
        static const Vector3i UnitY;
        static const Vector3i UnitZ;

        // Constructors

        constexpr Vector3i() = default;
        constexpr Vector3i(const glm::ivec3& vec) : glm::ivec3(vec) {}
        constexpr Vector3i(int x)                                   { this->x = x; this->y = x; this->z = x; }
        constexpr Vector3i(int x, int y, int z)                     { this->x = x; this->y = y; this->z = z; }

        // Utilities

        static float Length(const Vector3i& vec);
        float        Length() const;
        static float LengthSquared(const Vector3i& vec);
        float        LengthSquared() const;
        static float Distance(const Vector3i& from, const Vector3i& to);
        float        Distance(const Vector3i& to) const;
        static float DistanceSquared(const Vector3i& from, const Vector3i& to);
        float        DistanceSquared(const Vector3i& to) const;

        static Vector3i Lerp(const Vector3i& from, const Vector3i& to, float alpha);
        void            Lerp(const Vector3i& to, float alpha);
        static Vector3i Transform(const Vector3i& vec, const Matrix& mat);
        void            Transform(const Matrix& mat);
        static Vector3i Translate(const Vector3i& vec, const Vector3& dir, float dist);
        void            Translate(const Vector3& dir, float dist);

        // Converters

        Vector3 ToVector3() const;

        const glm::ivec3& ToGlmVec3i() const;
        glm::ivec3&       ToGlmVec3i();

        // Operators

        bool      operator==(const Vector3i& vec) const;
        bool      operator!=(const Vector3i& vec) const;
        Vector3i& operator=(const Vector3i& vec) = default;
        Vector3i& operator+=(const Vector3i& vec);
        Vector3i& operator-=(const Vector3i& vec);
        Vector3i& operator*=(const Vector3i& vec);
        Vector3i& operator*=(float scalar);
        Vector3i& operator/=(float scalar);
        Vector3i  operator+(const Vector3i& vec) const;
        Vector3i  operator-(const Vector3i& vec) const;
        Vector3i  operator*(const Vector3i& vec) const;
        Vector3i  operator*(float scalar) const;
        Vector3i  operator/(float scalar) const;
        Vector3i  operator-() const;
    };
}
