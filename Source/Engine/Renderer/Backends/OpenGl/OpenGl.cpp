#include "Framework.h"
#include "Engine/Renderer/Backends/OpenGl/OpenGl.h"

#include "Engine/Renderer/Base.h"

namespace Silent::Renderer
{
    void OpenGlRenderer::Initialize(SDL_Window& window)
    {
        _window = &window;

        // TODO

        if constexpr (IS_DEBUG)
        {
            InitializeDebugGui();
        }
    }

    void OpenGlRenderer::Deinitialize()
    {
        // TODO
    }

    void OpenGlRenderer::Update()
    {
        DrawDebugGui();
        DrawFrame();
    }

    void OpenGlRenderer::SignalResizedFramebuffer()
    {
        _isFramebufferResized = true;
    }

    void OpenGlRenderer::SaveScreenshot() const
    {
        // TODO
    }

    void OpenGlRenderer::SubmitGui(std::function<void()> drawFunc)
    {
        if constexpr (IS_DEBUG)
        {
            _guiDrawCalls.push_back(drawFunc);
        }
    }

    void OpenGlRenderer::InitializeDebugGui()
    {
        // TODO
    }

    void OpenGlRenderer::DrawDebugGui()
    {
        // TODO
    }

    void OpenGlRenderer::DrawFrame()
    {
        // TODO
    }
}
