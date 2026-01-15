#include "Framework.h"
#include "Renderer/Common/Objects/Scene/Shape2d.h"

#include "Renderer/Common/Constants.h"
#include "Renderer/Common/Enums.h"
#include "Renderer/Common/Objects/Primitive/Vertex2d.h"
#include "Renderer/Common/Utils.h"

namespace Silent::Renderer
{
    Shape2d Shape2d::CreateLine(const Vector2& from, const Vector2& to,
                                const Color& colorFrom, const Color& colorTo,
                                int depth, ScaleMode scaleMode, BlendMode blendMode)
    {
        constexpr float WIDTH = SCREEN_SPACE_RES.y / RETRO_SCREEN_SPACE_RES.y;

        auto dir    = Vector2::Normalize(Vector2(std::max(from.x, to.x), std::max(from.y, to.y)) -
                                         Vector2(std::min(from.x, to.x), std::min(from.y, to.y)));
        auto offset = Vector2(dir.y, dir.x) * WIDTH;

        return Shape2d
        {
            .Vertices =
            {
                Vertex2d{ from,          colorFrom },
                Vertex2d{ to,            colorTo   },
                Vertex2d{ to   + offset, colorTo   },
                Vertex2d{ from + offset, colorFrom }
            },
            .Depth   = depth,
            .ScaleMd = scaleMode,
            .BlendMd = blendMode
        };
    }

    Shape2d Shape2d::CreateLine(const Vector2i& from, const Vector2i& to,
                                const Color& colorFrom, const Color& colorTo,
                                int depth, ScaleMode scaleMode, BlendMode blendMode)
    {
        return Shape2d::CreateLine((from.ToVector2() / RETRO_SCREEN_SPACE_RES) * SCREEN_SPACE_RES,
                                   (to.ToVector2()   / RETRO_SCREEN_SPACE_RES) * SCREEN_SPACE_RES,
                                   colorFrom, colorTo,
                                   depth, scaleMode, blendMode);
    }

    Shape2d Shape2d::CreateTriangle(const Vector2& vert0, const Vector2& vert1, const Vector2& vert2,
                                    const Color& color0, const Color& color1, const Color& color2,
                                    int depth, ScaleMode scaleMode, BlendMode blendMode)
    {
        return Shape2d
        {
            .Vertices =
            {
                Vertex2d{ vert0, color0 },
                Vertex2d{ vert1, color1 },
                Vertex2d{ vert2, color2 }
            },
            .Depth   = depth,
            .ScaleMd = scaleMode,
            .BlendMd = blendMode
        };
    }

    Shape2d Shape2d::CreateTriangle(const Vector2i& vert0, const Vector2i& vert1, const Vector2i& vert2,
                                    const Color& color0, const Color& color1, const Color& color2,
                                    int depth, ScaleMode scaleMode, BlendMode blendMode)
    {
        return Shape2d::CreateTriangle((vert0.ToVector2() / RETRO_SCREEN_SPACE_RES) * SCREEN_SPACE_RES,
                                       (vert1.ToVector2() / RETRO_SCREEN_SPACE_RES) * SCREEN_SPACE_RES,
                                       (vert2.ToVector2() / RETRO_SCREEN_SPACE_RES) * SCREEN_SPACE_RES,
                                       color0, color1, color2,
                                       depth, scaleMode, blendMode);
    }

    Shape2d Shape2d::CreateQuad(const Vector2& vert0, const Vector2& vert1, const Vector2& vert2, const Vector2& vert3,
                                const Color& color0, const Color& color1, const Color& color2, const Color& color3,
                                int depth, ScaleMode scaleMode, BlendMode blendMode)
    {
        return Shape2d
        {
            .Vertices =
            {
                Vertex2d{ vert0, color0 },
                Vertex2d{ vert1, color1 },
                Vertex2d{ vert2, color2 },
                Vertex2d{ vert3, color3 }
            },
            .Depth   = depth,
            .ScaleMd = scaleMode,
            .BlendMd = blendMode
        };
    }

    Shape2d Shape2d::CreateQuad(const Vector2i& vert0, const Vector2i& vert1, const Vector2i& vert2, const Vector2i& vert3,
                                const Color& color0, const Color& color1, const Color& color2, const Color& color3,
                                int depth, ScaleMode scaleMode, BlendMode blendMode)
    {
        return Shape2d::CreateQuad((vert0.ToVector2() / RETRO_SCREEN_SPACE_RES) * SCREEN_SPACE_RES,
                                   (vert2.ToVector2() / RETRO_SCREEN_SPACE_RES) * SCREEN_SPACE_RES,
                                   (vert3.ToVector2() / RETRO_SCREEN_SPACE_RES) * SCREEN_SPACE_RES,
                                   (vert1.ToVector2() / RETRO_SCREEN_SPACE_RES) * SCREEN_SPACE_RES,
                                   color0, color2, color3, color1,
                                   depth, scaleMode, blendMode);
    }
}
