#pragma once

namespace Silent::Renderer
{
    /** @brief Render stages representing pipelines or shader programs, depending on the renderer backend. */
    enum class RenderStage
    {
        // =========
        // Material
        // =========

        Primitive2d,
        Primitive2dTextured,
        //GlyphGradient, // @todo

        // =============
        // Post-process
        // =============

        Vignette,

        Count
    };

    /** @brief Render stage blending modes. */
    enum class BlendMode
    {
        Opaque,
        Alpha,
        FastAlpha,
        Multiply,
        Add,
        Subtract,
        Wireframe,

        Count
    };

    /** @brief Screen space shape align modes. */
    enum class AlignMode
    {
        Center,
        CenterTop,
        CenterBottom,
        CenterLeft,
        CenterRight,
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };

    /** @brief Screen space shape scale modes relative to the display aspect ratio. */
    enum class ScaleMode
    {
        Fit,
        Fill,
        Stretch
    };

    /** @brief Text embellishment styles. */
    enum class TextStyle
    {
        Flat,
        Gradient
    };
}
