#pragma once

#include "Renderer/Common/Constants.h"
#include "Renderer/Common/Enums.h"
#include "Renderer/Common/Resources/Primitive/Primitive2d.h"
#include "Renderer/Common/Resources/Primitive/Primitive3d.h"
#include "Renderer/Common/Resources/Scene/Shape2d.h"
#include "Renderer/Common/Resources/Scene/Sprite2d.h"
#include "Renderer/Common/Resources/Scene/Text2d.h"
#include "Renderer/Common/Resources/Scene/Triangle3d.h"
#include "Renderer/Common/Resources/MeshCache.h"
#include "Renderer/Common/Resources/TextureCache.h"
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

            std::vector<Primitive2d>           ImmediatePrimitives2d = {};
            std::vector<Primitive3d>           ImmediatePrimitives3d = {};
            std::vector<std::function<void()>> DebugGuiDrawCalls     = {};

            std::vector<std::string> TextureUploadQueue  = {}; /** Asset names. */
            std::vector<std::string> TextureReleaseQueue = {}; /** Asset names. */
            std::vector<std::string> MeshUploadQueue     = {}; /** Asset names. */
            std::vector<std::string> MeshReleaseQueue    = {}; /** Asset names. */
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
        bool         _isResized  = false;
        Color        _clearColor = Color::Clear;
        View         _view       = View();

        DoubleBuffer                      _doubleBuffer = {};
        std::unique_ptr<TextureCacheBase> _textures     = nullptr;
        std::unique_ptr<MeshCacheBase>    _meshes       = nullptr;

        std::mutex _primitives2dMutex = {};
        std::mutex _primitives3dMutex = {};

        std::vector<Shape2d>    _shapes2d    = {}; // } @todo Not really renderer objects. Should be part of an external system.
        std::vector<Sprite2d>   _sprites2d   = {}; // }
        std::vector<Glyph2d>    _glyphs2d    = {}; // }
        std::vector<Triangle3d> _triangles3d = {}; // }

    public:
        // =============
        // Constructors
        // =============

        /** @brief Creates a default uninitialized instance. @todo Not needed? */
        RendererBase() = default;

        // ========
        // Getters
        // ========

        /** @brief Gets the renderer backend type.
         *
         * @return Renderer backend type.
         */
        RendererType GetType() const;

        /** @brief Gets the draw call count for the previous render tick.
         *
         * @return Draw call count.
         */
        int GetDrawCallCount() const;

        /** @brief Gets the screen render resolution in pixels.
         *
         * @return Render resolution.
         */
        Vector2i GetViewportResolution() const;

        /** @brief Gets the screen aspect ratio.
         *
         * @return Screen aspect ratio.
         */
        float GetViewportAspectRatio() const;

        // @todo Shouldn't use outside the renderer?
        const BoundingFrustum& GetViewFrustum() const;

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

        /** @brief Updates the render view. */
        void UpdateView();

        /** @brief Processes high-level object data and swaps the double buffer for a new frame. */
        void PrepareFrameData();

        /** @brief Signals a viewport resize. */
        void SignalResize();

        /** @brief Queues a texture upload to the GPU.
         *
         * @param assetName Texture asset name.
         */
        void QueueTextureUpload(const std::string& assetName);

        /** @brief Queues a texture release from the GPU.
         *
         * @param assetName Texture asset name.
         */
        void QueueTextureRelease(const std::string& assetName);

        /** @brief Queues a mesh upload to the GPU.
         *
         * @param assetName Mesh asset name.
         */
        void QueueMeshUpload(const std::string& assetName);

        /** @brief Queues a mesh unload from the GPU.
         *
         * @param assetName Mesh asset name.
         */
        void QueueMeshRelease(const std::string& assetName);

        /** @brief Submits an immediate-mode 2D untextured screen shape for drawing.
         *
         * @param prim 2D untextured screen shape to draw.
         * @return `true` if the 2D untextured screen shape was successfully submitted, `false` otherwise.
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

        /** @brief Submits an immediate-mode 3D textured triangle for drawing.
         *
         * @param tri 3D textured triangle to draw.
         * @return `true` if the 3D textured triangle was successfully submitted, `false` otherwise.
         */
        bool SubmitTriangle3d(const Triangle3d& tri);

        /** @brief Initializes the renderer and its subsystems.
         *
         * @param window Window to claim as the render surface.
         */
        virtual void Initialize(SDL_Window& window) = 0;

        /** @brief Gracefully deinitializes the renderer and its subsystems. */
        virtual void Deinitialize() = 0;

        /** @brief Sets up GPU resources for a new frame. */
        virtual void PrepareFrameResources() = 0;

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
        void SubmitDebugTriangle(const Vector2& vert0, const Vector2& vert1, const Vector2& vert2,
                                 const Color& color, ScaleMode scaleMode, Debug::Page page);

        /** @brief Submits a 3D triangle polygon with additive blending for drawing.
         * Used to construct more complex geometry.
         *
         * @param vert0 First vertex.
         * @param vert1 Second vertex.
         * @param vert2 Third vertex.
         * @param color Triangle color.
         * @param page Debug page in which the triangle will be visible.
         */
        void SubmitDebugTriangle(const Vector3& vert0, const Vector3& vert1, const Vector3& vert2,
                                 const Color& color, Debug::Page page);

    protected:
        // ========
        // Helpers
        // ========

        /** @brief Initializes the double buffer. */
        void InitializeDoubleBuffer();

        /** @brief Processes immediate-mode 2D sprites into 2D primitives. */
        void ProcessSprites2d();

        /** @brief Processes immediate-mode 2D shapes into 2D primitives. */
        void ProcessShapes2d();

        /** @brief Processes immediate-mode 2D glyphs into 2D primitives. */
        void ProcessGlyphs2d();

        /** @brief Processes immediate-mode 3D triangles into 3D primitives. */
        void ProcessTriangles3d();

        /** @brief Sorts render buffer data in the double buffer.
         * Called at the start of `Update`.
         */
        void SortRenderBufferData();

        /** @brief Draws the current frame according to a hardcoded render graph. */
        void DrawFrame();

        /** @brief Draws a 3D scene to a cleared off-screen render texture.
         * Called before `Draw2dScene`.
         */
        virtual void Draw3dScene() = 0;

        /** @brief Draws dithering over the 3D scene to an off-screen render texture.
         * Called after `Draw3dScene` and before `Draw2dScene`.
         */
        virtual void DrawDither() = 0;

        /** @brief Draws a 2D scene on top of the 3D scene to an off-screen render texture.
         * Called after `Draw3dScene` and before `DrawPostProcess`.
         */
        virtual void Draw2dScene() = 0;

        /** @brief Draws post-process effects on top of the combined 3D and 2D scenes to an off-screen render texture.
         * Called after `Draw2dScene` and before `DrawViewport`.
         */
        virtual void DrawPostProcess() = 0;

        /** @brief Draws the viewport containing post-procesed, combined 3D and 2D scenes to the swapchain.
         * Called after `DrawPostProcess` and before `DrawPowerMenu`.
         */
        virtual void DrawViewport() = 0 ;

        /** @brief Draws a power menu on top of the viewport to the swapchain.
         * Called after `DrawViewport`.
         */
        virtual void DrawPowerMenu() = 0;
    };

    /** @brief Creates a renderer of a specified backend type.
     *
     * @param type Renderer backend type.
     * @return New renderer.
     */
    std::unique_ptr<RendererBase> CreateRenderer(RendererType type);
}
