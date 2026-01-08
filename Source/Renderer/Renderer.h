#pragma once

#include "Renderer/Common/Constants.h"
#include "Renderer/Common/Enums.h"
#include "Renderer/Common/Objects/Primitive2d.h"
#include "Renderer/Common/Objects/Primitive3d.h"
#include "Renderer/Common/Objects/Scene/Glyph.h"
#include "Renderer/Common/Objects/Scene/Sprite2d.h"
#include "Renderer/Common/Objects/Scene/Text.h"
#include "Renderer/Common/Texture.h"
#include "Renderer/Common/View.h"

namespace Silent::Renderer
{
    /** @brief Renderer backend types. */
    enum class RendererType
    {
        SdlGpu
    };

    /** @brief Data for double-buffering. */
    struct DataBuffer
    {
        std::vector<Primitive3d>            Primitives3d      = {};
        std::vector<Primitive2d>            Primitives2d      = {};
        std::vector<Sprite2d>               Sprites2d         = {};
        std::vector<Glyph>                  Glyphs            = {};
        std::vector<Primitive2d>            DebugPrimitives2d = {};
        std::vector<Primitive3d>            DebugPrimitives3d = {};
        std::vector<std::function<void()>>  DebugGuiDrawCalls = {};
    };

    /** @brief Renderer base. */
    class RendererBase
    {
    protected:
        // =======
        // Fields
        // =======

        RendererType _type          = RendererType::SdlGpu;
        SDL_Window*  _window        = nullptr;
        View         _view          = View();
        Color        _clearColor    = Color::Clear;
        int          _drawCallCount = 0; // @todo Probably belongs in `DataBuffer` as well.
        bool         _isResized     = false;

        std::unique_ptr<TextureManagerBase> _textures = nullptr;

        // @todo Cleaner way to manage this.
        DataBuffer _activeBuffer = {};
        DataBuffer _renderBuffer = {};

    public:
        // =============
        // Constructors
        // =============

        /** @brief Constructs an uninitialized default `RendererBase`. @todo Not needed? */
        RendererBase() = default;

        // ========
        // Getters
        // ========

        /** @brief Gets the renderer backend type.
         *
         * @return Renderer backend type.
         */
        RendererType GetType() const;

        /** @brief Gets the screen render resolution in pixels.
         *
         * @return Render resolution.
         */
        Vector2i GetScreenResolution() const;

        /** @brief Gets the screen aspect ratio.
         *
         * @return Screen aspect ratio.
         */
        float GetScreenAspectRatio() const;

        /** @brief Gets the draw call count for the current render tick.
         *
         * @return Draw call count.
         */
        int GetDrawCallCount() const;

        // ========
        // Setters
        // ========

        /** @brief Sets the color used to clear the screen.
         *
         * @param color New clear color.
         */
        void SetClearColor(const Color& color);

        // ==========
        // Utilities
        // ==========

        /** @brief Updates the render data buffer for the current frame and clears the active data buffer for the next update. */
        void UpdateRenderDataBuffer();

        /** @brief Signals a viewport resize. */
        void SignalResize();

        /** @brief Submits a 2D primitive for drawing.
         *
         * @param prim 2D primitive to draw.
         */
        void Submit2dPrimitive(const Primitive2d& prim);

        /** @brief Submits a glyph for drawing.
         *
         * @param glyph Glyph to draw.
         */
        void SubmitGlyph(const Glyph& glyph);

        /** @brief Submits a screen sprite for drawing.
         *
         * @param assedIdx Asset index containing the sprite to draw.
         * @param uvMin Minimum UV position as alpha (top-left).
         * @param uvMax Maximum UV position as alpha (bottom-right).
         * @param pos Screen position in percent.
         * @param rot Sprite rotation.
         * @param scale Sprite scale.
         * @param color Tint color and opacity.
         * @param depth Draw priority. Lower values take precedence.
         * @param alignMode Sprite align mode relative to the screen aspect ratio.
         * @param scaleMode Sprite scale mode relative to the screen aspect ratio.
         * @param blendMode Draw blend mode.
         */
        void SubmitScreenSprite(int assetIdx, const Vector2& uvMin, const Vector2& uvMax, const Vector2& pos, short rot, const Vector2& scale,
                                const Color& color, int depth, AlignMode alignMode, ScaleMode scaleMode, BlendMode blendMode);

        /** @brief Initializes the renderer and its subsystems.
         *
         * @param window Window to claim as the render surface.
         */
        virtual void Initialize(SDL_Window& window) = 0;

        /** @brief Gracefully deinitializes the renderer and its subsystems. */
        virtual void Deinitialize() = 0;

        /** @brief Prepares all GPU data and draws to the render surface. */
        virtual void Update() = 0;

        /** @brief Saves a screenshot of the active render surface to the designated `Screenshots` folder on the system. */
        virtual void SaveScreenshot() const = 0;

        // ======
        // Debug
        // ======

        /** @brief Submits a debug message for drawing.
         *
         * @param msg Message string.
         * @param pos Screen position in percent.
         * @param color Color.
         * @param alignMode Align mode.
         */
        void SubmitDebugText(const std::string& msg, const Vector2& pos, const Color& color, TextAlignMode alignMode = TextAlignMode::Left);

        /** @brief Submits a function used to construct an ImGui power menu for drawing.
         *
         * @param drawFunc Function defining a power menu.
         */
        void SubmitDebugGui(std::function<void()> drawFunc);

        /** @brief Submits a 2D line with additive blending for drawing.
         * Used to construct more complex geometry.
         *
         * @param from Start point.
         * @param to End point.
         * @param color Line color.
         * @param scaleMode Screen space scale mode.
         * @param page Debug page in which the line will be visible.
         */
        void SubmitDebugLine(const Vector2& from, const Vector2& to, const Color& color, ScaleMode scaleMode, Debug::Page page);

        /** @brief Submits a 3D line with additive blending for drawing.
         * Used to construct more complex geometry.
         *
         * @param from Start point.
         * @param to End point.
         * @param color Line color.
         * @param page Debug page in which the line will be visible.
         */
        void SubmitDebugLine(const Vector3& from, const Vector3& to, const Color& color, Debug::Page page);

        /** @brief Submits a 2D triangle polygon with additive blending for drawing.
         * Used to construct more complex geometry.
         *
         * @param vert0 First vertex.
         * @param vert1 Second vertex.
         * @param vert2 Third vertex.
         * @param color Triangle color.
         * @param scaleMode Screen space scale mode.
         * @param page Debug page in which the triangle will be visible.
         */
        void SubmitDebugTriangle(const Vector2& vert0, const Vector2& vert1, const Vector2& vert2, const Color& color, ScaleMode scaleMode, Debug::Page page);

        /** @brief Submits a 3D triangle polygon with additive blending for drawing.
         * Used to construct more complex geometry.
         *
         * @param vert0 First vertex.
         * @param vert1 Second vertex.
         * @param vert2 Third vertex.
         * @param color Triangle color.
         * @param page Debug page in which the triangle will be visible.
         */
        void SubmitDebugTriangle(const Vector3& vert0, const Vector3& vert1, const Vector3& vert2, const Color& color, Debug::Page page);

    protected:
        // ========
        // Helpers
        // ========

        /** @brief Prepares renderer buffer data used for the current frame. Called at the start of `Update`. */
        void PrepareRenderBufferData();

        /** @brief Draws a 3D scene. Called before `Draw2dScene`. */
        virtual void Draw3dScene() = 0;

        /** @brief Draws a 2D scene on top of the 3D scene. Called after `Draw3dScene` and before `DrawPostProcess`. */
        virtual void Draw2dScene() = 0;

        /** @brief Draws post-process effects on top of the 3D and 2D scene. Called after `Draw2dScene` and before `DrawDebugGui`. */
        virtual void DrawPostProcess() = 0;

        /** @brief Draws a power menu on top of the post-processed 3D and 2D scenes. Called after `DrawPostProcess`. */
        virtual void DrawDebugGui() = 0;
    };

    /** @brief Creates a renderer of a specified backend type.
     *
     * @param type Renderer backend type.
     * @return New renderer.
     */
    std::unique_ptr<RendererBase> CreateRenderer(RendererType type);
}
