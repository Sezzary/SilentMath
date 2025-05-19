#include "Framework.h"
#include "Engine/Renderer/Renderer.h"

#include "Engine/Renderer/Backends/OpenGl/OpenGl.h"
#include "Engine/Renderer/Backends/Vulkan/Vulkan.h"
#include "Engine/Renderer/Base.h"

namespace Silent::Renderer
{
    std::unique_ptr<RendererBase> g_Renderer = nullptr;

    std::unique_ptr<RendererBase> CreateRenderer(RendererType type)
    {
        switch (type)
        {
            case RendererType::OpenGl:
            {
                return std::make_unique<OpenGlRenderer>();
            }

            case RendererType::Vulkan:
            {
                return std::make_unique<VulkanRenderer>();
            }

            default:
            {
                break;
            }
        }

        throw std::runtime_error("Attempted to create unsupported renderer type" + std::to_string((int)type) + ".");
    }
}
