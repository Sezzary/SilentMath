#pragma once

#include "Math/Constants.h"

namespace Silent::Math
{
    class Matrix;

    class Vector3 : public glm::vec3
    {
    public:
        // Constants

        static constexpr uint AXIS_COUNT = 3;

        // Presets

        static const Vector3 Zero;
        static const Vector3 One;
        static const Vector3 UnitX;
        static const Vector3 UnitY;
        static const Vector3 UnitZ;

        // Constructors

        constexpr Vector3() = default;
        constexpr Vector3(const glm::vec3& vec) : glm::vec3(vec) {}
        constexpr Vector3(float x)                               { this->x = x; this->y = x; this->z = x; }
        constexpr Vector3(float x, float y, float z)             { this->x = x; this->y = y; this->z = z; }

        // Utilities

        static bool    Compare(const Vector3& vec0, const Vector3& vec1, float epsilon = EPSILON);
        static float   Length(const Vector3& vec);
        float          Length() const;
        static float   LengthSquared(const Vector3& vec);
        float          LengthSquared() const;
        static float   Distance(const Vector3& from, const Vector3& to);
        float          Distance(const Vector3& to) const;
        static float   DistanceSquared(const Vector3& from, const Vector3& to);
        float          DistanceSquared(const Vector3& to) const;
        static float   Dot(const Vector3& vec0, const Vector3& vec1);
        float          Dot(const Vector3& vec) const;
        static Vector3 Cross(const Vector3& vec0, const Vector3& vec1);
        Vector3        Cross(const Vector3& vec) const;

        static Vector3 Min(const Vector3& vec0, const Vector3& vec1);
        void           Min(const Vector3& vec);
        static Vector3 Max(const Vector3& vec0, const Vector3& vec1);
        void           Max(const Vector3& vec);
        static Vector3 Normalize(const Vector3& vec);
        void           Normalize();
        static Vector3 Lerp(const Vector3& from, const Vector3& to, float alpha);
        void           Lerp(const Vector3& to, float alpha);
        static Vector3 Smoothstep(const Vector3& from, const Vector3& to, float alpha);
        void           Smoothstep(const Vector3& to, float alpha);
        static Vector3 Transform(const Vector3& vec, const Matrix& mat);
        void           Transform(const Matrix& mat);
        static Vector3 Rotate(const Vector3& vec, const Matrix& mat);
        void           Rotate(const Matrix& mat);
        static Vector3 Translate(const Vector3& vec, const Vector3& dir, float dist);
        void           Translate(const Vector3& dir, float dist);

        // Converters

        const glm::vec3& ToGlmVec3() const;
        glm::vec3&       ToGlmVec3();

        // Operators

        bool     operator==(const Vector3& vec) const;
        bool     operator!=(const Vector3& vec) const;
        Vector3& operator=(const Vector3& vec) = default;
        Vector3& operator+=(const Vector3& vec);
        Vector3& operator-=(const Vector3& vec);
        Vector3& operator*=(const Vector3& vec);
        Vector3& operator*=(float scalar);
        Vector3& operator/=(float scalar);
        Vector3  operator+(const Vector3& vec) const;
        Vector3  operator-(const Vector3& vec) const;
        Vector3  operator*(const Vector3& vec) const;
        Vector3  operator*(float scalar) const;
        Vector3  operator/(float scalar) const;
        Vector3  operator-() const;
    };
}
