#pragma once

#include "Renderer/Common/Constants.h"
#include "Renderer/Common/Enums.h"
#include "Renderer/Common/Objects/Primitive3d.h"
#include "Renderer/Common/Objects/Primitive/Primitive2d.h"
#include "Renderer/Common/Objects/Scene/Shape2d.h"
#include "Renderer/Common/Objects/Scene/Sprite2d.h"
#include "Renderer/Common/Objects/Scene/Text2d.h"
#include "Renderer/Common/Texture.h"
#include "Renderer/Common/View.h"

namespace Silent::Renderer
{
    /** @brief Renderer backend types. */
    enum class RendererType
    {
        SdlGpu
    };

    /** @brief Double-buffered scene data. */
    struct DoubleBuffer
    {
        struct Data
        {
            int DrawCallCount = 0;

            std::vector<Primitive2d> Primitives2d = {}; // @todo Sprites, shapes, and texts processed into this.

            std::vector<Shape2d>               Shapes2d          = {}; // } @todo Not really renderer objects. Should be external.
            std::vector<Sprite2d>              Sprites2d         = {}; // }
            std::vector<Glyph2d>               Glyphs2d          = {};
            std::vector<std::function<void()>> DebugGuiDrawCalls = {};

            std::vector<Primitive3d>           Primitives3d      = {};
            std::vector<Shape2d>               DebugShapes2d     = {};
            std::vector<Primitive3d>           DebugPrimitives3d = {};
        };

        Data Active = {};
        Data Render = {};
    };

    /** @brief Renderer base. */
    class RendererBase
    {
    protected:
        // =======
        // Fields
        // =======

        RendererType _type       = RendererType::SdlGpu;
        SDL_Window*  _window     = nullptr;
        View         _view       = View();
        Color        _clearColor = Color::Clear;
        bool         _isResized  = false;

        DoubleBuffer                        _doubleBuffer = {};
        std::unique_ptr<TextureManagerBase> _textures     = nullptr;

        std::mutex _gpuMutex = {};

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

        /** @brief Gets the draw call count for the previous render tick.
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

        /** @brief Swaps the double buffer and clears active data for new updates on the next tick. */
        void SwapDoubleBuffer();

        /** @brief Signals a viewport resize. */
        void SignalResize();

        /** @brief Submits an immediate-mode 2D screen shape for drawing.
         *
         * @param prim 2D screen shape to draw.
         * @return `true` if the 2D screen shape was successfully submitted, `false` otherwise.
         */
        bool SubmitShape2d(const Shape2d& shape);

        /** @brief Submits an immediate-mode 2D screen sprite for drawing.
         *
         * @param sprite 2D screen sprite to draw.
         * @return `true` if the 2D screen sprite was successfully submitted, `false` otherwise.
         */
        bool SubmitSprite2d(const Sprite2d& sprite);

        /** @brief Submits immediate-mode 2D screen text glyphs for drawing.
         *
         * @param text 2D screen text to draw.
         * @return `true` if the 2D screen text glyphs were successfully submitted, `false` otherwise.
         */
        bool SubmitText2d(const Text2d& text);

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

        /** @brief Initializes the double buffer. */
        void InitializeDoubleBuffer();

        // @todo Process these into 2D primitives.
        void ProcessShapes2d();
        void ProcessSprites2d();
        void ProcessTexts2d();

        /** @brief Sorts render data in the double buffer, preparing it for batching and parsing to GPU buffer data.
         * Called at the start of `Update`.
         */
        void SortRenderBufferData();

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
