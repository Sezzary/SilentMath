#pragma once

namespace Silent::Math
{
    class Color
    {
    public:
        // Fields

        uchar R = 0;
        uchar G = 0;
        uchar B = 0;
        uchar A = 0;

        // Presets

        static const Color Black;
        static const Color Red;
        static const Color Green;
        static const Color Blue;
        static const Color White;

        // Constructors

        constexpr	 Color() = default;
        constexpr	 Color(uchar r, uchar g, uchar b, uchar a = 255) : R(r), G(g), B(b), A(a) {}
        static Color FromPackedRgba(uint packedRgba);

        // Utilities

        Color Invert() const;

        static Color Blend(const Color& color0, const Color& color1, uchar alpha);
        static Color Brighten(const Color& color, uchar alpha);
        static Color Darken(const Color& color, uchar alpha);

        // Converters

        int ToPackedRgba() const;

        // Operators

        bool   operator ==(const Color& color) const;
        bool   operator !=(const Color& color) const;
        Color& operator =(const Color& color) = default;
    };
}
