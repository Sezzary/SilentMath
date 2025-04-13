#pragma once

namespace Silent::Math
{
    class Color
    {
    public:
        // Fields

        unsigned char R = 0;
        unsigned char G = 0;
        unsigned char B = 0;
        unsigned char A = 0;

        // Presets

        static const Color Black;
        static const Color Red;
        static const Color Green;
        static const Color Blue;
        static const Color White;

        // Constructors

        constexpr	 Color() = default;
        constexpr	 Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255) : R(r), G(g), B(b), A(a) {}
        static Color FromPackedRgba(unsigned int packedRgba);
        static Color FromHsl(short h, unsigned char s, unsigned char l);

        // Utilities

        Color Invert() const;

        static Color Blend(const Color& color0, const Color& color1, unsigned char alpha);
        static Color Brighten(const Color& color, unsigned char alpha);
        static Color Darken(const Color& color, unsigned char alpha);

        // Converters

        int ToPackedRgba() const;

        // Operators

        bool   operator ==(const Color& color) const;
        bool   operator !=(const Color& color) const;
        Color& operator =(const Color& color) = default;
    };
}
