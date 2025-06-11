#pragma once

#include "Math/Constants.h"

namespace Silent::Math
{
    class Vector4;

    class Color : glm::vec4
    {
    public:
        // Accessors

        const float& R() const;
        float&       R();
        uchar        R8() const;

        const float& G() const;
        float&       G();
        uchar        G8() const;

        const float& B() const;
        float&       B();
        uchar        B8() const;

        const float& A() const;
        float&       A();
        uchar        A8() const;

        // Presets

        static const Color Black;
        static const Color White;
        static const Color Red;
        static const Color Green;
        static const Color Blue;

        // Constructors

        constexpr Color()                                          : glm::vec4(1.0f, 0.0f, 0.0f, 0.0f) {}
        //constexpr Color(const glm::vec4& color)                    : glm::vec4(color) {}
        constexpr Color(float r, float g, float b, float a = 1.0f) : glm::vec4(a, r, g, b) {}

        static Color From8Bit(uchar r, uchar g, uchar b, uchar a = FP_COLOR(1.0f));
        static Color FromPackedRgba(uint packedRgba);

        // Utilities

        static Color Lerp(const Color& color0, const Color& color1, float alpha);
        void         Lerp(const Color& color, float alpha);
        static Color Invert(const Color& color);
        void         Invert();
        static Color Blend(const Color& color0, const Color& color1, float alpha);
        void         Blend(const Color& color, float alpha);
        static Color Brighten(const Color& color, float factor);
        void         Brighten(float alpha);
        static Color Darken(const Color& color, float factor);
        void         Darken(float alpha);

        // Converters

        uint ToPackedRgba() const;

        const Vector4&   ToVector4() const;
        Vector4&         ToVector4();
        const glm::vec4& ToGlmVec4() const;
        glm::vec4&       ToGlmVec4();

        // Operators

        bool   operator ==(const Color& color) const;
        bool   operator !=(const Color& color) const;
        Color& operator =(const Color& color) = default;
    };
}
