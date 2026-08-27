#include "Framework.h"
#include "Renderer/Common/Resources/Scene/Sprite2d.h"

#include "Application.h"
#include "Assets/AssetStreamer.h"
#include "Renderer/Common/Constants.h"
#include "Renderer/Common/Enums.h"
#include "Renderer/Common/Resources/Scene/Shape2d.h"
#include "Renderer/Common/Utils.h"
#include "Renderer/Renderer.h"

using namespace Silent::Assets;

namespace Silent::Renderer
{
    static float GetSpriteAspectRatio(const std::string& texName)
    {
        auto& assets = g_App.GetAssets();

        const auto* asset = assets[texName];
        switch (asset->Type)
        {
            case AssetType::Tim:
            {
                const auto& data = asset->GetData<TimAsset>();
                return data->AspectRatio;
            }
            case AssetType::Png:
            {
                const auto& data = asset->GetData<PngAsset>();
                return data->AspectRatio;
            }
            default:
            {
                Debug::Log(Fmt("Attempted to get sprite aspect ratio from non-image asset `{}`.", texName),
                           Debug::LogLevel::Error);
                break;
            }
        }

        return 0.0f;
    }

    Sprite2d Sprite2d::CreateSprite2d(const std::string& texName, const Vector2& uvMin, const Vector2& uvMax,
                                      const Vector2& pos, float rot, const Vector2& scale,
                                      const Color& color0, const Color& color1, const Color& color2, const Color& color3, int paletteIdx,
                                      int depth, AlignMode alignMode, ScaleMode scaleMode,
                                      BlendMode blendMode)
    {
        return Sprite2d
        {
            .TextureName = texName,
            .UvMin       = uvMin,
            .UvMax       = uvMax,
            .Position    = pos,
            .Rotation    = rot,
            .Scale       = scale,
            .Col0        = color0,
            .Col1        = color1,
            .Col2        = color2,
            .Col3        = color3,
            .PaletteIdx  = paletteIdx,
            .Depth       = depth,
            .AlignMd     = alignMode,
            .ScaleMd     = scaleMode,
            .BlendMd     = blendMode
        };
    }

    Sprite2d Sprite2d::CreateSprite2d(const std::string& texName, const Vector2& uvMin, const Vector2& uvMax,
                                      const Vector2& pos, float rot, const Vector2& scale, const Color& color, int paletteIdx,
                                      int depth, AlignMode alignMode, ScaleMode scaleMode,
                                      BlendMode blendMode)
    {
        return Sprite2d
        {
            .TextureName = texName,
            .UvMin       = uvMin,
            .UvMax       = uvMax,
            .Position    = pos,
            .Rotation    = rot,
            .Scale       = scale,
            .Col0        = color,
            .Col1        = color,
            .Col2        = color,
            .Col3        = color,
            .PaletteIdx  = paletteIdx,
            .Depth       = depth,
            .AlignMd     = alignMode,
            .ScaleMd     = scaleMode,
            .BlendMd     = blendMode
        };
    }

    Sprite2d Sprite2d::CreateSprite2d(const std::string& texName, const Vector2& uvMin, const Vector2& uvMax,
                                      const Vector2& pos, float rot, float scale, const Color& color, int paletteIdx,
                                      int depth, AlignMode alignMode, ScaleMode scaleMode,
                                      BlendMode blendMode)
    {
        const auto& renderer = g_App.GetRenderer();

        float viewportAspect = renderer.GetViewportAspectRatio();
        auto  spriteAspect   = GetSpriteAspectRatio(texName);

        auto localScale = Vector2::One;
        switch (scaleMode)
        {
            case ScaleMode::VerticalEdge:
            {
                localScale = Vector2(spriteAspect, 1.0f);
                break;
            }
            case ScaleMode::HorizontalEdge:
            {
                localScale = Vector2(1.0f, 1.0f / spriteAspect) * viewportAspect;
                break;
            }
            case ScaleMode::Fit:
            {
                if (spriteAspect >= viewportAspect)
                {
                    localScale = Vector2(1.0f, 1.0f / spriteAspect) * viewportAspect;
                }
                else
                {
                    localScale.x = spriteAspect;
                }
                break;
            }
            case ScaleMode::Fill:
            {
                if (spriteAspect >= viewportAspect)
                {
                    localScale.x = spriteAspect;
                }
                else
                {
                    localScale = Vector2(1.0f, 1.0f / spriteAspect) * viewportAspect;
                }
                break;
            }
            default:
            {
                break;
            }
        }

        return Sprite2d
        {
            .TextureName = texName,
            .UvMin       = uvMin,
            .UvMax       = uvMax,
            .Position    = pos,
            .Rotation    = rot,
            .Scale       = localScale * scale,
            .Col0        = color,
            .Col1        = color,
            .Col2        = color,
            .Col3        = color,
            .PaletteIdx  = paletteIdx,
            .Depth       = depth,
            .AlignMd     = alignMode,
            .ScaleMd     = scaleMode,
            .BlendMd     = blendMode
        };
    }

    Sprite2d CreateSprite2d(const std::string& texName, const Vector2i& pixelMin, const Vector2i& pixelMax,
                            const Vector2i& pos, float rot, int paletteIdx,
                            int depth, AlignMode alignMode, ScaleMode scaleMode,
                            BlendMode blendMode)
    {
        return Sprite2d
        {
            .TextureName = texName,
            .UvMin       = ConvertRetroScreenPixelsToPercent(pixelMin) / SCREEN_SPACE_RES,
            .UvMax       = ConvertRetroScreenPixelsToPercent(pixelMax) / SCREEN_SPACE_RES,
            .Position    = ConvertRetroScreenPixelsToPercent(pos),
            .Rotation    = rot,
            .Col0        = Color::White,
            .Col1        = Color::White,
            .Col2        = Color::White,
            .Col3        = Color::White,
            .PaletteIdx  = paletteIdx,
            .Depth       = depth,
            .AlignMd     = alignMode,
            .ScaleMd     = scaleMode,
            .BlendMd     = blendMode
        };
    }
}
