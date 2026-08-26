#pragma once

namespace Silent::Renderer
{
    /** @brief Render stages representing pipelines or shader programs, depending on the backend. */
    enum class RenderStage
    {
        // =========
        // Material
        // =========

        Shape2d,
        Sprite2d,
        Glyph2d,
        Model,

        // =============
        // Post-process
        // =============

        Dither,
        LumaFade,
        FilmGrain,
        Vignette,
        Crt,
        Fxaa,

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

    /** @brief Render target types. */
    enum class RenderTargetType
    {
        Virtual240p,
        Virtual480p,
        Native,

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
        VerticalEdge,   /** Fit. */
        HorizontalEdge, /** Fill. */
        Stretch
    };
}
