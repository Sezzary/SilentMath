#include "Framework.h"
#include "Engine/Renderer/Renderer.h"

namespace Silent::Renderer
{
    RendererManager g_Renderer = RendererManager();

    bool QueueFamilyIndices::IsComplete()
    {
        return GraphicsFamily != NO_VALUE && PresentFamily != NO_VALUE;
    }

    void RendererManager::InitializeImGui()
    {
        constexpr uint DEFAULT_SIZE = 1000;

        VkDescriptorPoolSize poolSizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER,                DEFAULT_SIZE },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DEFAULT_SIZE },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,          DEFAULT_SIZE },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,          DEFAULT_SIZE },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,   DEFAULT_SIZE },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,   DEFAULT_SIZE },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,         DEFAULT_SIZE },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,         DEFAULT_SIZE },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, DEFAULT_SIZE },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, DEFAULT_SIZE },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,       DEFAULT_SIZE }
        };

        auto poolInfo              = VkDescriptorPoolCreateInfo{};
        poolInfo.sType             = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.flags             = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        poolInfo.maxSets           = DEFAULT_SIZE;
        poolInfo.poolSizeCount     = std::size(poolSizes);
        poolInfo.pPoolSizes        = poolSizes;
        vkCreateDescriptorPool(_device, &poolInfo, nullptr, &_descPool);

        ImGui::CreateContext();
        ImGui_ImplSDL3_InitForVulkan(_window);

        auto idxs = FindQueueFamilies(_physicalDevice);

        auto initInfo           = ImGui_ImplVulkan_InitInfo{};
        initInfo.Instance       = _instance;
        initInfo.PhysicalDevice = _physicalDevice;
        initInfo.Device         = _device;
        initInfo.QueueFamily    = idxs.GraphicsFamily;
        initInfo.Queue          = _graphicsQueue;
        initInfo.PipelineCache  = nullptr;
        initInfo.DescriptorPool = _descPool;
        initInfo.RenderPass     = _renderPass;
        initInfo.MinImageCount  = _swapChainImages.size();
        initInfo.ImageCount     = _swapChainImages.size();
        initInfo.MSAASamples    = VK_SAMPLE_COUNT_1_BIT;
        ImGui_ImplVulkan_Init(&initInfo);

        auto cmd = BeginSingleTimeCommands();
        ImGui_ImplVulkan_CreateFontsTexture();
        EndSingleTimeCommands(cmd);
    }

    void RendererManager::Initialize(SDL_Window& window)
    {
        _window = &window;

        CreateInstance();
        SetupDebugMessenger();
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
        CreateSwapChain();
        CreateImageViews();
        CreateRenderPass();
        CreateGraphicsPipeline();
        CreateFramebuffers();
        CreateCommandPool();
        CreateCommandBuffers();
        CreateSyncObjects();
        
        if constexpr (IS_DEBUG)
        {
            InitializeImGui();
        }
    }

    void RendererManager::Deinitialize()
    {
        vkDeviceWaitIdle(_device);

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(_device, _renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(_device, _imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(_device, _inFlightFences[i], nullptr);
        }

        vkDestroyCommandPool(_device, _commandPool, nullptr);

        for (auto framebuffer : _swapChainFramebuffers)
        {
            vkDestroyFramebuffer(_device, framebuffer, nullptr);
        }

        vkDestroyPipeline(_device, _graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
        vkDestroyRenderPass(_device, _renderPass, nullptr);

        for (auto imageView : _swapChainImageViews)
        {
            vkDestroyImageView(_device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(_device, _swapChain, nullptr);
        vkDestroyDevice(_device, nullptr);

        if (ENABLE_VALIDATION_LAYERS)
        {
            DestroyDebugUtilsMessengerExt(_instance, _debugMessenger, nullptr);
        }

        vkDestroySurfaceKHR(_instance, _surface, nullptr);
        vkDestroyInstance(_instance, nullptr);
    }

    void RendererManager::Update()
    {
        DrawGui();
        DrawFrame();
    }
    
    void RendererManager::SubmitGui(std::function<void()> drawFunc)
    {
        if constexpr (IS_DEBUG)
        {
            _guiDrawCalls.push_back(drawFunc);
        }
    }

    VkInstance RendererManager::GetInstance()
    {
        return _instance;
    }

    std::vector<const char*> RendererManager::GetRequiredExtensions()
    {
        uint  sdlExtCount = 0;
        auto* sdlExts     = SDL_Vulkan_GetInstanceExtensions(&sdlExtCount);

        auto exts = std::vector<const char*>(sdlExts, sdlExts + sdlExtCount);
        if (ENABLE_VALIDATION_LAYERS)
        {
            exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return exts;
    }

    bool RendererManager::CheckValidationLayerSupport() const
    {
        uint layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    
        auto availableLayers = std::vector<VkLayerProperties>(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    
        for (const char* layerName : VALIDATION_LAYERS)
        {
            bool isLayerFound = false;
            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layerName, layerProperties.layerName) == 0)
                {
                    isLayerFound = true;
                    break;
                }
            }
    
            if (!isLayerFound)
            {
                return false;
            }
        }
    
        return true;
    }

    bool RendererManager::CheckDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint extCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, nullptr);
    
        auto availableExts = std::vector<VkExtensionProperties>(extCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, availableExts.data());
    
        auto requiredExts = std::set<std::string>(DEVICE_EXTENSIONS.begin(), DEVICE_EXTENSIONS.end());
        for (const auto& ext : availableExts)
        {
            requiredExts.erase(ext.extensionName);
        }
    
        return requiredExts.empty();
    }

    bool RendererManager::IsDeviceSuitable(VkPhysicalDevice device)
    {
        /*auto deviceProperties = VkPhysicalDeviceProperties{};
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        auto deviceFeatures = VkPhysicalDeviceFeatures{};
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);*/

        // TODO: Pick best GPU.

        bool isExtSupported = CheckDeviceExtensionSupport(device);

        bool isSwapChainAdequate = false;
        if (isExtSupported)
        {
            auto swapChainSupport = QuerySwapChainSupport(device);
            isSwapChainAdequate   = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
        }

        return FindQueueFamilies(device).IsComplete() && isExtSupported && isSwapChainAdequate;
    }

    void RendererManager::PickPhysicalDevice()
    {
        uint deviceCount = 0;
        vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
        if (deviceCount == 0)
        {
            throw std::runtime_error("Failed to find GPUs with Vulkan support.");
        }

        auto devices = std::vector<VkPhysicalDevice>(deviceCount);
        vkEnumeratePhysicalDevices(_instance, &deviceCount, devices.data());
        for (const auto& device : devices)
        {
            if (IsDeviceSuitable(device))
            {
                _physicalDevice = device;
                break;
            }
        }
        
        if (_physicalDevice == VK_NULL_HANDLE)
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    VkSurfaceFormatKHR RendererManager::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        return availableFormats.front();
    }

    VkPresentModeKHR RendererManager::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
    {
        for (const auto& availablePresentMode : availablePresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D RendererManager::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
    {
        if (capabilities.currentExtent.width != UINT_MAX)
        {
            return capabilities.currentExtent;
        }

        int width = 0;
        int height = 0;
        SDL_GetWindowSize(_window, &width, &height);

        auto actualExtent = VkExtent2D
        {
            std::clamp((uint)width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
            std::clamp((uint)height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
        };
        return actualExtent;
    }

    QueueFamilyIndices RendererManager::FindQueueFamilies(VkPhysicalDevice device)
    {
        auto idxs = QueueFamilyIndices {};

        uint queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        auto queueFamilies = std::vector<VkQueueFamilyProperties>(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        for (int i = 0; i < queueFamilies.size(); i++)
        {
            const auto& queueFamily = queueFamilies[i];

            // Check for graphics queue family.
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                idxs.GraphicsFamily = i;
            }

            // Check for present queue family.
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);
            if (presentSupport)
            {
                idxs.PresentFamily = i;
            }

            // TODO: Could prefer physical device that supports drawing and presentation in the same queue for improved performance.

            if (idxs.IsComplete())
            {
                break;
            }
        }

        return idxs;
    }

    SwapChainSupportDetails RendererManager::QuerySwapChainSupport(VkPhysicalDevice device)
    {
        auto details = SwapChainSupportDetails{};

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.Capabilities);

        uint formatCount = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);
        if (formatCount != 0)
        {
            details.Formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details.Formats.data());
        }

        uint presentModeCount = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, nullptr);
        if (presentModeCount != 0)
        {
            details.PresentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, details.PresentModes.data());
        }

        return details;
    }

    void RendererManager::DrawFrame()
    {
        vkWaitForFences(_device, 1, &_inFlightFences[_currentFrame], VK_TRUE, UINT64_MAX);
        vkResetFences(_device, 1, &_inFlightFences[_currentFrame]);

        uint32 imageIdx = 0;
        vkAcquireNextImageKHR(_device, _swapChain, UINT64_MAX, _imageAvailableSemaphores[_currentFrame], VK_NULL_HANDLE, &imageIdx);

        vkResetCommandBuffer(_commandBuffers[_currentFrame], 0);
        RecordCommandBuffer(_commandBuffers[_currentFrame], imageIdx);

        VkSemaphore          waitSemaphores[]   = { _imageAvailableSemaphores[_currentFrame] };
        VkSemaphore          signalSemaphores[] = { _renderFinishedSemaphores[_currentFrame] };
        VkPipelineStageFlags waitStages[]       = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

        auto submitInfo                 = VkSubmitInfo{};
        submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount   = 1;
        submitInfo.pWaitSemaphores      = waitSemaphores;
        submitInfo.pWaitDstStageMask    = waitStages;
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &_commandBuffers[_currentFrame];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores    = signalSemaphores;

        if (vkQueueSubmit(_graphicsQueue, 1, &submitInfo, _inFlightFences[_currentFrame]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to submit draw command buffer.");
        }

        VkSwapchainKHR swapChains[] = { _swapChain };

        auto presentInfo               = VkPresentInfoKHR{};
        presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores    = signalSemaphores;
        presentInfo.swapchainCount     = 1;
        presentInfo.pSwapchains        = swapChains;
        presentInfo.pImageIndices      = &imageIdx;
        presentInfo.pResults           = nullptr; // Optional.
        vkQueuePresentKHR(_presentQueue, &presentInfo);

        _currentFrame = (_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void RendererManager::DrawGui()
    {
        ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        // Draw GUIs.
        for (auto& drawFunc : _guiDrawCalls)
        {
            drawFunc();
        }
        _guiDrawCalls.clear();

        // DEMO
        ImGui::ShowDemoWindow();

        ImGui::Render();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), _commandBuffers[_currentFrame]);
    }

    void RendererManager::CreateInstance()
    {
        if (ENABLE_VALIDATION_LAYERS && !CheckValidationLayerSupport())
        {
            throw std::runtime_error("Validation layers requested but unavailable.");
        }

        // Create application info.
        auto appInfo = VkApplicationInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        // Get SDL extensions.
        auto sdlExts = GetRequiredExtensions();

        // Create instance create info.
        auto createInfo = VkInstanceCreateInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = (uint32)sdlExts.size();
        createInfo.ppEnabledExtensionNames = sdlExts.data();

        auto debugCreateInfo = VkDebugUtilsMessengerCreateInfoEXT{};
        if (ENABLE_VALIDATION_LAYERS)
        {
            Log("Vulkan validation layers enabled.");

            createInfo.enabledLayerCount = (uint32)VALIDATION_LAYERS.size();
            createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();

            PopulateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
        }
        else
        {
            createInfo.enabledLayerCount = 0;
        }

        // Get available Vulkan extension count.
        uint extCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
        
        // Get available Vulkan extensions.
        auto exts = std::vector<VkExtensionProperties>(extCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extCount, exts.data());

        // TODO: Check if all required SDL extensions are available.

        // Log available Vulkan extensions.
        Log("Available Vulkan extensions (" + std::to_string(extCount) + "):");
        for (const auto& ext : exts)
        {
            Log(std::string("    ") + ext.extensionName);
        }

        if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan instance.");
        }
    }

    void RendererManager::CreateLogicalDevice()
    {
        auto idxs = FindQueueFamilies(_physicalDevice);
        auto queueCreateInfos = std::vector<VkDeviceQueueCreateInfo>{};
        auto uniqueQueueFamilies = std::set<uint>{ idxs.GraphicsFamily, idxs.PresentFamily };
        
        float queuePriority = 1.0f;
        for (uint queueFamily : uniqueQueueFamilies)
        {
            auto queueCreateInfo = VkDeviceQueueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        auto deviceFeatures = VkPhysicalDeviceFeatures{};

        auto createInfo = VkDeviceCreateInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = (uint32)queueCreateInfos.size();
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = (uint32)DEVICE_EXTENSIONS.size();
        createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

        if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create logical device.");
        }

        vkGetDeviceQueue(_device, idxs.GraphicsFamily, 0, &_graphicsQueue);
        vkGetDeviceQueue(_device, idxs.PresentFamily, 0, &_presentQueue);
    }

    void RendererManager::CreateSurface()
    {
        if (!SDL_Vulkan_CreateSurface(_window, _instance, nullptr, &_surface))
        {
            throw std::runtime_error("Failed to create window surface.");
        }
    }

    void RendererManager::CreateSwapChain()
    {
        auto swapChainSupport = QuerySwapChainSupport(_physicalDevice);
    
        auto surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.Formats);
        auto presentMode = ChooseSwapPresentMode(swapChainSupport.PresentModes);
        auto extent = ChooseSwapExtent(swapChainSupport.Capabilities);

        // Specify image count.
        uint imageCount = swapChainSupport.Capabilities.minImageCount + 1;
        if (swapChainSupport.Capabilities.maxImageCount > 0 &&
            imageCount > swapChainSupport.Capabilities.maxImageCount)
        {
            imageCount = swapChainSupport.Capabilities.maxImageCount;
        }

        auto createInfo = VkSwapchainCreateInfoKHR{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = _surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        auto idxs = FindQueueFamilies(_physicalDevice);
        uint queueFamilyIdxs[] = { idxs.GraphicsFamily, idxs.PresentFamily };
        if (idxs.GraphicsFamily != idxs.PresentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIdxs;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0; // Optional.
            createInfo.pQueueFamilyIndices = nullptr; // Optional.
        }

        createInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(_device, &createInfo, nullptr, &_swapChain) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create swap chain.");
        }

        vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, nullptr);
        _swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(_device, _swapChain, &imageCount, _swapChainImages.data());
        
        _swapChainImageFormat = surfaceFormat.format;
        _swapChainExtent = extent;
    }

    void RendererManager::CreateImageViews()
    {
        _swapChainImageViews.resize(_swapChainImages.size());
        for (int i = 0; i < _swapChainImages.size(); i++)
        {
            auto createInfo = VkImageViewCreateInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = _swapChainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = _swapChainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(_device, &createInfo, nullptr, &_swapChainImageViews[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create image views.");
            }
        }
    }

    void RendererManager::CreateRenderPass()
    {
        auto colorAttachment = VkAttachmentDescription{};
        colorAttachment.format = _swapChainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        auto colorAttachmentRef = VkAttachmentReference{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        
        auto subpass = VkSubpassDescription{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        auto dependency = VkSubpassDependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        auto renderPassInfo = VkRenderPassCreateInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies = &dependency;
        
        if (vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create render pass.");
        }
    }

    void RendererManager::CreateGraphicsPipeline()
    {
        auto vertShaderCode = ReadFile("Shaders/Vert.spv");
        auto fragShaderCode = ReadFile("Shaders/Frag.spv");

        auto vertShaderModule = CreateShaderModule(vertShaderCode);
        auto fragShaderModule = CreateShaderModule(fragShaderCode);

        auto vertShaderStageInfo = VkPipelineShaderStageCreateInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        auto fragShaderStageInfo = VkPipelineShaderStageCreateInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

        auto vertexInputInfo = VkPipelineVertexInputStateCreateInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional.
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional.

        auto inputAssembly = VkPipelineInputAssemblyStateCreateInfo{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        auto viewport = VkViewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)_swapChainExtent.width;
        viewport.height = (float)_swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        auto scissor = VkRect2D{};
        scissor.offset = { 0, 0 };
        scissor.extent = _swapChainExtent;

        auto dynamicStates = std::vector<VkDynamicState>
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        
        auto dynamicState = VkPipelineDynamicStateCreateInfo{};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = (uint32)dynamicStates.size();
        dynamicState.pDynamicStates = dynamicStates.data();

        auto viewportState = VkPipelineViewportStateCreateInfo{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        auto rasterizer = VkPipelineRasterizationStateCreateInfo{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f; // Optional.
        rasterizer.depthBiasClamp = 0.0f; // Optional.
        rasterizer.depthBiasSlopeFactor = 0.0f; // Optional.

        auto multisampling = VkPipelineMultisampleStateCreateInfo{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f; // Optional.
        multisampling.pSampleMask = nullptr; // Optional.
        multisampling.alphaToCoverageEnable = VK_FALSE; // Optional.
        multisampling.alphaToOneEnable = VK_FALSE; // Optional.

        auto colorBlendAttachment = VkPipelineColorBlendAttachmentState{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional.
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional.
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional.
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional.
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional.
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional.
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        auto colorBlending = VkPipelineColorBlendStateCreateInfo{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional.
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f; // Optional.
        colorBlending.blendConstants[1] = 0.0f; // Optional.
        colorBlending.blendConstants[2] = 0.0f; // Optional.
        colorBlending.blendConstants[3] = 0.0f; // Optional.

        auto pipelineLayoutInfo = VkPipelineLayoutCreateInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0; // Optional.
        pipelineLayoutInfo.pSetLayouts = nullptr; // Optional.
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional.
        pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional.

        if (vkCreatePipelineLayout(_device, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create pipeline layout.");
        }

        auto pipelineInfo = VkGraphicsPipelineCreateInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = shaderStages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = nullptr; // Optional.
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = _pipelineLayout;
        pipelineInfo.renderPass = _renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional.
        pipelineInfo.basePipelineIndex = NO_VALUE; // Optional.

        if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_graphicsPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create graphics pipeline.");
        }

        vkDestroyShaderModule(_device, fragShaderModule, nullptr);
        vkDestroyShaderModule(_device, vertShaderModule, nullptr);
    }

    void RendererManager::CreateFramebuffers()
    {
        _swapChainFramebuffers.resize(_swapChainImageViews.size());
        for (int i = 0; i < _swapChainImageViews.size(); i++)
        {
            VkImageView attachments[] =
            {
                _swapChainImageViews[i]
            };
        
            auto framebufferInfo = VkFramebufferCreateInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = _renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = _swapChainExtent.width;
            framebufferInfo.height = _swapChainExtent.height;
            framebufferInfo.layers = 1;
        
            if (vkCreateFramebuffer(_device, &framebufferInfo, nullptr, &_swapChainFramebuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create framebuffer.");
            }
        }
    }
    
    void RendererManager::CreateCommandPool()
    {
        auto queueFamilyIdxs = FindQueueFamilies(_physicalDevice);

        auto poolInfo = VkCommandPoolCreateInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIdxs.GraphicsFamily;

        if (vkCreateCommandPool(_device, &poolInfo, nullptr, &_commandPool) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create command pool.");
        }
    }

    void RendererManager::CreateCommandBuffers()
    {
        _commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        auto allocInfo               = VkCommandBufferAllocateInfo{};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool        = _commandPool;
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32)_commandBuffers.size();
        
        if (vkAllocateCommandBuffers(_device, &allocInfo, _commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate command buffers.");
        }
    }

    void RendererManager::CreateSyncObjects()
    {
        _imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        _renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        _inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        auto semaphoreInfo  = VkSemaphoreCreateInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        auto fenceInfo  = VkFenceCreateInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            if (vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(_device, &fenceInfo, nullptr, &_inFlightFences[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create semaphores.");
            }
        }
    }

    VkShaderModule RendererManager::CreateShaderModule(const std::vector<char>& code)
    {
        auto createInfo = VkShaderModuleCreateInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32*>(code.data());

        auto shaderModule = VkShaderModule{};
        if (vkCreateShaderModule(_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create shader module.");
        }

        return shaderModule;
    }

    void RendererManager::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32 imageIdx)
    {
        auto beginInfo = VkCommandBufferBeginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = 0; // Optional.
        beginInfo.pInheritanceInfo = nullptr; // Optional.
        
        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to begin recording command buffer.");
        }

        auto clearColor = VkClearValue{ { { 0.0f, 0.0f, 0.0f, 1.0f } } };

        auto renderPassInfo = VkRenderPassBeginInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = _renderPass;
        renderPassInfo.framebuffer = _swapChainFramebuffers[imageIdx];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = _swapChainExtent;
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        if constexpr (IS_DEBUG)
        {
            ImGui::Render();
        }

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // App draw.
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _graphicsPipeline);

        auto viewport = VkViewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)_swapChainExtent.width;
        viewport.height = (float)_swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        
        auto scissor = VkRect2D{};
        scissor.offset = { 0, 0 };
        scissor.extent = _swapChainExtent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        if constexpr (IS_DEBUG)
        {
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
        }

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to record command buffer.");
        }
    }

    void RendererManager::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo = VkDebugUtilsMessengerCreateInfoEXT{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
        createInfo.pUserData = nullptr;
    }

    void RendererManager::SetupDebugMessenger()
    {
        if (!ENABLE_VALIDATION_LAYERS)
        {
            return;
        }

        auto createInfo = VkDebugUtilsMessengerCreateInfoEXT{};
        PopulateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerExt(_instance, &createInfo, nullptr, &_debugMessenger) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to set up debug messenger.");
        }
    }

    VkCommandBuffer RendererManager::BeginSingleTimeCommands()
    {
        auto allocInfo               = VkCommandBufferAllocateInfo{};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool        = _commandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer = nullptr;
        vkAllocateCommandBuffers(_device, &allocInfo, &commandBuffer);

        auto beginInfo  = VkCommandBufferBeginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void RendererManager::EndSingleTimeCommands(VkCommandBuffer commandBuffer)
    {
        vkEndCommandBuffer(commandBuffer);

        auto submitInfo               = VkSubmitInfo{};
        submitInfo.sType              = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers    = &commandBuffer;
        vkQueueSubmit(_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

        vkQueueWaitIdle(_graphicsQueue);
        vkFreeCommandBuffers(_device, _commandPool, 1, &commandBuffer);
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL RendererManager::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                                           VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                                           const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                                           void* pUserData)
    {
        // TODO: Log level.
        Log("Validation layer: " + std::string(pCallbackData->pMessage));
        return VK_FALSE;
    }

    VkResult CreateDebugUtilsMessengerExt(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerExt(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            func(instance, debugMessenger, pAllocator);
        }
    }

    std::vector<char> ReadFile(const std::string& fileName)
    {
        auto file = std::ifstream(fileName, std::ios::ate | std::ios::binary);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open file.");
        }

        uint fileSize = (uint)file.tellg();
        auto buffer = std::vector<char>(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }
}
