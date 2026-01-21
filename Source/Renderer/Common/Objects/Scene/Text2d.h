#pragma once

#include "Renderer/Common/Objects/Scene/Sprite2d.h"
#include "Renderer/Common/Enums.h"

namespace Silent::Renderer
{
    /** @brief Text embellishment styles. */
    enum class TextStyle
    {
        Flat,
        Gradient
    };

    /** @brief 2D screen text with markup. */
    struct Text2d
    {
        std::string Message      = {};
        std::string FontName     = {};
        Vector2     Position     = Vector2::Zero;
        float       Rotation     = 0.0f;
        float       Scale        = 0.0f;
        float       KerningScale = 0.0f;
        float       Opacity      = 0.0f;
        TextStyle   Style        = TextStyle::Flat;
        bool        HasShadow    = false;
        int         Depth        = 0;
        AlignMode   AlignMd      = AlignMode::Center;
        ScaleMode   ScaleMd      = ScaleMode::ShortEdge;
        BlendMode   BlendMd      = BlendMode::Alpha;

        static Text2d CreateText2d(const std::string& msg, const std::string& fontName,
                                   const Vector2& pos, float rot, float scale, float kernScale, float opacity,
                                   TextStyle style, bool hasDropShadow,
                                   int depth = 0, AlignMode alignMode = AlignMode::Center, ScaleMode scaleMode = ScaleMode::ShortEdge,
                                   BlendMode blendMode = BlendMode::Alpha);
    };
}
