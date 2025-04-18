#pragma once

#include "Math/Constants.h"

namespace Silent::Math
{
    class Color : glm::vec4
    {
    public:
        // Presets

        static const Color Black;
        static const Color White;
        static const Color Red;
        static const Color Green;
        static const Color Blue;

        // Constructors

        constexpr	 Color() = default;
        constexpr	 Color(float r, float g, float b, float a = 1.0f) { x = r; y = g; z = b; w = a; }
        static Color FromPackedRgba(uint packedRgba);

        // Getters

        const float& R() const;
        float&       R();
        const float& G() const;
        float&       G();
        const float& B() const;
        float&       B();
        const float& A() const;
        float&       A();

        // Utilities

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

        // Operators

        bool   operator ==(const Color& color) const;
        bool   operator !=(const Color& color) const;
        Color& operator =(const Color& color) = default;
    };
}
