#pragma once

#include "Renderer/Common/Enums.h"

namespace Silent::Renderer
{
    constexpr int SPRITE_2D_COUNT_MAX = 1024;

    /** @brief 2D screen sprite with texture and color tint. */
    struct Sprite2d
    {
        std::string TextureName = {};
        Vector2     UvMin       = Vector2::Zero;
        Vector2     UvMax       = Vector2::Zero;
        Vector2     Position    = Vector2::Zero;
        float       Rotation    = 0.0f;
        Vector2     Scale       = Vector2::Zero;
        Color       Col0        = Color::Clear;
        Color       Col1        = Color::Clear;
        Color       Col2        = Color::Clear;
        Color       Col3        = Color::Clear;
        int         Depth       = 0;
        AlignMode   AlignMd     = AlignMode::Center;
        ScaleMode   ScaleMd     = ScaleMode::ShortEdge;
        BlendMode   BlendMd     = BlendMode::Opaque;

        static Sprite2d CreateSprite2d(const std::string& texName, const Vector2& uvMin, const Vector2& uvMax,
                                       const Vector2& pos, float rot, const Vector2& scale,
                                       const Color& color0, const Color& color1, const Color& color2, const Color& color3,
                                       int depth = 0, AlignMode alignMode = AlignMode::Center, ScaleMode scaleMode = ScaleMode::ShortEdge,
                                       BlendMode blendMode = BlendMode::Alpha);

        static Sprite2d CreateSprite2d(const std::string& texName, const Vector2& uvMin, const Vector2& uvMax,
                                       const Vector2& pos, float rot, const Vector2& scale, const Color& color,
                                       int depth = 0, AlignMode alignMode = AlignMode::Center, ScaleMode scaleMode = ScaleMode::ShortEdge,
                                       BlendMode blendMode = BlendMode::Alpha);

        static Sprite2d CreateSprite2d(const std::string& texName, const Vector2i& pixelMin, const Vector2i& pixelMax,
                                       const Vector2& pos, float rot,
                                       int depth = 0, AlignMode alignMode = AlignMode::Center, ScaleMode scaleMode = ScaleMode::ShortEdge,
                                       BlendMode blendMode = BlendMode::Alpha);
    };
}
