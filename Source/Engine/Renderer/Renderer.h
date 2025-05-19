#pragma once

#include "Engine/Renderer/Backends/Vulkan/Vulkan.h"

namespace Silent::Renderer
{
    enum class RendererType
    {
        OpenGl,
        Vulkan
    };

    extern std::unique_ptr<RendererBase> g_Renderer;

    std::unique_ptr<RendererBase> CreateRenderer(RendererType type);
}
