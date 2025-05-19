#pragma once

#include "Engine/Renderer/Base.h"

namespace Silent::Renderer
{
    enum class RendererType
    {
        OpenGl,
        Vulkan
    };

    std::unique_ptr<RendererBase> CreateRenderer(RendererType type);
}
