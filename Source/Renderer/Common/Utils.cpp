#include "Framework.h"
#include "Renderer/Common/Utils.h"

#include "Application.h"
#include "Renderer/Common/Constants.h"
#include "Renderer/Common/Enums.h"
#include "Renderer/Renderer.h"

namespace Silent::Renderer
{
    Vector2 GetAlignmentPivot(AlignMode alignMode)
    {
        switch (alignMode)
        {
            default:
            case AlignMode::Center:
            {
                return Vector2(0.5f, 0.5f);
            }
            case AlignMode::CenterTop:
            {
                return Vector2(0.5f, 0.0f);
            }
            case AlignMode::CenterBottom:
            {
                return Vector2(0.5f, 1.0f);
            }
            case AlignMode::CenterLeft:
            {
                return Vector2(0.0f, 0.5f);
            }
            case AlignMode::CenterRight:
            {
                return Vector2(1.0f, 0.5f);
            }
            case AlignMode::TopLeft:
            {
                return Vector2(0.0f, 0.0f);
            }
            case AlignMode::TopRight:
            {
                return Vector2(0.0f, 1.0f);
            }
            case AlignMode::BottomLeft:
            {
                return Vector2(1.0f, 0.0f);
            }
            case AlignMode::BottomRight:
            {
                return Vector2(1.0f, 1.0f);
            }
        }

        return Vector2::Zero;
    }

    Vector2 GetScreenAspectCorrection(ScaleMode scaleMode)
    {
        const auto& renderer = g_App.GetRenderer();

        float aspect = renderer.GetScreenAspectRatio();

        auto aspectCorrection = Vector2::One;
        switch(scaleMode)
        {
            case ScaleMode::ShortEdge:
            {
                if (aspect >= 1.0f)
                {
                    aspectCorrection.x = 1.0f / aspect;
                }
                else
                {
                    aspectCorrection.y = 1.0f / (1.0f / aspect);
                }
                break;
            }
            case ScaleMode::LongEdge:
            {
                if (aspect >= 1.0f)
                {
                    aspectCorrection.y = 1.0f / (1.0f / aspect);
                }
                else
                {
                    aspectCorrection.x = 1.0f / aspect;
                }
                break;
            }
            default:
            case ScaleMode::Stretch:
            {
                break;
            }
        }
        
        return aspectCorrection;
    }

    Vector2 GetAspectCorrectScreenPosition(const Vector2 pos, ScaleMode scaleMode)
    {
        if (scaleMode == ScaleMode::Stretch)
        {
            return pos;
        }

        const auto& renderer = g_App.GetRenderer();

        auto  screenRes       = renderer.GetScreenResolution().ToVector2();
        float screenResAspect = screenRes.x / screenRes.y;
        
        // @todo Needs another adjustment.
        return pos * GetScreenAspectCorrection(scaleMode);
    }

    Vector2 ConvertRetroScreenPixelsToPercent(const Vector2i& pos)
    {
        return (pos.ToVector2() / RETRO_SCREEN_SPACE_RES) * SCREEN_SPACE_RES;
    }

    Vector2i ConvertScreenPercentToRetroPixels(const Vector2& pos)
    {
        return Vector2i((pos / SCREEN_SPACE_RES) * RETRO_SCREEN_SPACE_RES);
    }

    Vector2 ConvertScreenPercentToNdc(const Vector2& pos)
    {
        return Vector2(((pos.x * 2) / SCREEN_SPACE_RES.x) - 1.0f,
                       1.0f - ((pos.y * 2) / SCREEN_SPACE_RES.y));
    }

    Vector2 ConvertNdcToScreenPercent(const Vector2& ndc)
    {
        return Vector2(((ndc.x + 1.0f) * SCREEN_SPACE_RES.x) / 2,
                       ((1.0f - ndc.y) * SCREEN_SPACE_RES.y) / 2);
    }

    Vector2i NormalizeRetroScreenPosition(const Vector2i pos)
    {
        return Vector2i((pos.x < 0) ? (((int)RETRO_SCREEN_SPACE_RES.x / 2) + pos.x) : pos.x,
                        (pos.y < 0) ? (((int)RETRO_SCREEN_SPACE_RES.y / 2) + pos.y) : pos.y);
    }
}
