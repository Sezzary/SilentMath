#pragma once
#include "Renderer/Common/Objects/Scene/Sprite2d.h"
#include "Renderer/Common/Enums.h"

namespace Silent::Renderer
{
    /** @brief 2D glyph screen sprite. */
    struct Glyph
    {
        Sprite2d  Sprite        = {};
        TextStyle Style         = TextStyle::Flat;
        bool      HasDropShadow = false;
        // @todo Need to track baseline or centre for the gradient to be aligned properly across glyphs in the same line.
        
        static Glyph CreateGlyph(const std::string& texName, const Vector2& uvMin, const Vector2& uvMax,
                                 const Vector2& pos, float rot, const Vector2& scale, const Color& color,
                                 TextStyle style, bool hasDropShadow,
                                 int depth = 0, AlignMode alignMode = AlignMode::Center, ScaleMode scaleMode = ScaleMode::Fit, BlendMode blendMode = BlendMode::Alpha);
    };
}
