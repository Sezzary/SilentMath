#pragma once

#include "Renderer/Common/Enums.h"
#include "Renderer/Common/Resources/Primitive/Vertex2d.h"

namespace Silent::Renderer
{
    constexpr int SHAPE_2D_COUNT_MAX = 128;

    /** @brief 2D screen shape representing a line, triangle, or quad. */
    struct Shape2d
    {
        std::vector<Vertex2d> Vertices = {};
        int                   Depth    = 0;
        ScaleMode             ScaleMd  = ScaleMode::ShortEdge;
        BlendMode             BlendMd  = BlendMode::Alpha;

        /** @brief Creates a 2D line shape with a width of 1 retro pixel (320x240 resolution) using screen positions in percent.
         *
         * @param from Start position in screen percent.
         * @param to End position in screen percent.
         * @param colorFrom Start color.
         * @param colorTo End color.
         * @param depth Render priority.
         * @return 2D line shape.
         */
        static Shape2d CreateLine(const Vector2& from, const Vector2& to,
                                  const Color& colorFrom, const Color& colorTo,
                                  int depth = 0, ScaleMode scaleMode = ScaleMode::ShortEdge, BlendMode blendMode = BlendMode::Alpha);

        /** @brief Creates a 2D line shape with a width of 1 retro pixel (320x240 resolution) using screen positions in retro pixels.
         *
         * @param from Start position in retro pixels (320x240 resolution).
         * @param to End position in retro pixels (320x240 resolution).
         * @param colorFrom Start color.
         * @param colorTo End color.
         * @param depth Render priority.
         * @return 2D line shape.
         */
        static Shape2d CreateLine(const Vector2i& from, const Vector2i& to,
                                  const Color& colorFrom, const Color& colorTo,
                                  int depth = 0, ScaleMode scaleMode = ScaleMode::ShortEdge, BlendMode blendMode = BlendMode::Alpha);

        /** @brief Creates a 2D triangle shape using screen positions in percent.
         *
         * @param vert0 First vertex position in screen percent.
         * @param vert1 Second vertex position in screen percent.
         * @param vert2 Third vertex position in screen percent.
         * @param color0 First vertex color.
         * @param color1 Second vertex color.
         * @param color2 Third vertex color.
         * @param depth Render priority.
         * @return 2D triangle shape.
         */
        static Shape2d CreateTriangle(const Vector2& vert0, const Vector2& vert1, const Vector2& vert2,
                                      const Color& color0, const Color& color1, const Color& color2,
                                      int depth = 0, ScaleMode scaleMode = ScaleMode::ShortEdge, BlendMode blendMode = BlendMode::Alpha);

        /** @brief Creates a 2D triangle shape using retro pixels.
         *
         * @param vert0 First vertex position in retro pixels (320x240 resolution).
         * @param vert1 Second vertex position in retro pixels (320x240 resolution).
         * @param vert2 Third vertex position in retro pixels (320x240 resolution).
         * @param color0 First vertex color.
         * @param color1 Second vertex color.
         * @param color2 Third vertex color.
         * @param depth Render priority.
         * @return 2D triangle shape.
         */
        static Shape2d CreateTriangle(const Vector2i& vert0, const Vector2i& vert1, const Vector2i& vert2,
                                      const Color& color0, const Color& color1, const Color& color2,
                                      int depth = 0, ScaleMode scaleMode = ScaleMode::ShortEdge, BlendMode blendMode = BlendMode::Alpha);

        /** @brief Creates a 2D quad shape.
         *
         * @param vert0 First vertex position in screen percent.
         * @param vert1 Second vertex position in screen percent.
         * @param vert2 Third vertex position in screen percent.
         * @param vert3 Fourth vertex position in screen percent.
         * @param color0 First vertex color.
         * @param color1 Second vertex color.
         * @param color2 Third vertex color.
         * @param color3 Fourth vertex color.
         * @param depth Render priority.
         * @return 2D quad shape.
         */
        static Shape2d CreateQuad(const Vector2& vert0, const Vector2& vert1, const Vector2& vert2, const Vector2& vert3,
                                  const Color& color0, const Color& color1, const Color& color2, const Color& color3,
                                  int depth = 0, ScaleMode scaleMode = ScaleMode::ShortEdge, BlendMode blendMode = BlendMode::Alpha);

        /** @brief Creates a 2D quad shape using retro pixels.
         *
         * @note Vertex order: top-left, bottom-left, top-right, bottom-right.
         *
         * @param vert0 First vertex position in retro pixels (320x240 resolution).
         * @param vert1 Second vertex position in retro pixels (320x240 resolution).
         * @param vert2 Third vertex position in retro pixels (320x240 resolution).
         * @param vert3 Fourth vertex position in retro pixels (320x240 resolution).
         * @param color0 First vertex color.
         * @param color1 Second vertex color.
         * @param color2 Third vertex color.
         * @param color3 Fourth vertex color.
         * @param depth Render priority.
         * @return 2D quad shape.
         */
        static Shape2d CreateQuad(const Vector2i& vert0, const Vector2i& vert1, const Vector2i& vert2, const Vector2i& vert3,
                                  const Color& color0, const Color& color1, const Color& color2, const Color& color3,
                                  int depth = 0, ScaleMode scaleMode = ScaleMode::ShortEdge, BlendMode blendMode = BlendMode::Alpha);
    };
}
