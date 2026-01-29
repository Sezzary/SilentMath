#pragma once

namespace Silent::Renderer
{
    /** @brief Render stages representing pipelines or shader programs, depending on the backend. */
    enum class RenderStage
    {
        // ==========
        // Materials
        // ==========

        Sprite2d,
        Glyph2d,

        // ========
        // Effects
        // ========

        Dither,
        Fade,
        Crt,
        Vignette,

        // ======
        // Final
        // ======

        Blit,

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

    /** @brief Screen space scale modes relative to the display aspect ratio. */
    enum class ScaleMode
    {
        ShortEdge,
        LongEdge,
        HorizontalEdge, // } Might implement this, not sure.
        VerticalEdge,   // }
        Stretch
    };
}
