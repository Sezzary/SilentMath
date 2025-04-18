#include "Framework.h"
#include "Math/Objects/Color.h"

namespace Silent::Math
{
    const Color Color::Black = Color(FP_ALPHA(0.0f), FP_ALPHA(0.0f), FP_ALPHA(0.0f), FP_ALPHA(0.0f));
    const Color Color::White = Color(FP_ALPHA(1.0f), FP_ALPHA(1.0f), FP_ALPHA(1.0f), FP_ALPHA(1.0f));
    const Color Color::Red   = Color(FP_ALPHA(1.0f), FP_ALPHA(0.0f), FP_ALPHA(0.0f), FP_ALPHA(1.0f));
    const Color Color::Green = Color(FP_ALPHA(0.0f), FP_ALPHA(1.0f), FP_ALPHA(0.0f), FP_ALPHA(1.0f));
    const Color Color::Blue  = Color(FP_ALPHA(0.0f), FP_ALPHA(0.0f), FP_ALPHA(1.0f), FP_ALPHA(1.0f));

    Color Color::FromPackedRgba(uint packedRgba)
    {
        constexpr uchar BIT_MASK = FP_COLOR(1.0f);

        return Color((float)((packedRgba >> 24) & BIT_MASK) / (float)BIT_MASK,
                     (float)((packedRgba >> 16) & BIT_MASK) / (float)BIT_MASK,
                     (float)((packedRgba >> 8) & BIT_MASK) / (float)BIT_MASK,
                     (float)(packedRgba & BIT_MASK) / (float)BIT_MASK);
    }

    const float& Color::R() const
    {
        return x;
    }

    float& Color::R()
    {
        return x;
    }

    const float& Color::G() const
    {
        return y;
    }

    float& Color::G()
    {
        return y;
    }

    const float& Color::B() const
    {
        return z;
    }

    float& Color::B()
    {
        return z;
    }

    const float& Color::A() const
    {
        return w;
    }

    float& Color::A()
    {
        return w;
    }

    Color Color::Invert(const Color& color)
    {
        return Color(1.0f - color.R(),
                     1.0f - color.G(),
                     1.0f - color.B(),
                     1.0f - color.A());
    }

    void Color::Invert()
    {
        *this = Color::Invert(*this);
    }

    Color Color::Blend(const Color& color0, const Color& color1, float alpha)
    {
        auto invAlpha = 1.0f - alpha;
        return Color((color0.R() * invAlpha) + (color1.R() * alpha),
                     (color0.G() * invAlpha) + (color1.G() * alpha),
                     (color0.B() * invAlpha) + (color1.B() * alpha),
                     (color0.A() * invAlpha) + (color1.A() * alpha));
    }

    void Color::Blend(const Color& color, float alpha)
    {
        *this = Color::Blend(*this, color, alpha);
    }

    Color Color::Brighten(const Color& color, float factor)
    {
        return Color(std::min(color.R() * factor, 1.0f),
                     std::min(color.G() * factor, 1.0f),
                     std::min(color.B() * factor, 1.0f),
                     color.A());
    }

    void Color::Brighten(float alpha)
    {
        *this = Color::Brighten(*this, alpha);
    }

    Color Color::Darken(const Color& color, float factor)
    {
        return Color(color.R() * factor,
                     color.G() * factor,
                     color.B() * factor,
                     color.A());
    }

    void Color::Darken(float alpha)
    {
        *this = Color::Darken(*this, alpha);
    }

    // TODO: Check.
    uint Color::ToPackedRgba() const
    {
        return ((FP_COLOR(A()) << 24) | (FP_COLOR(R()) << 16) | (FP_COLOR(G()) << 8) | FP_COLOR(B()));
    }

    bool Color::operator ==(const Color& color) const
    {
        return R() == color.R() && G() == color.G() && B() == color.B() && A() == color.A();
    }

    bool Color::operator !=(const Color& color) const
    {
        return !(*this == color);
    }
}
