#include "Framework.h"
#include "Engine/Renderer/Renderer.h"

namespace Silent::Renderer
{
    bool QueueFamilyIndices::IsComplete()
    {
        return GraphicsFamily != NO_VALUE && PresentFamily != NO_VALUE;
    }

    void HelloTriangleApplication::Run()
    {
        InitializeWindow();
        InitializeVulkan();
        MainLoop();
        Cleanup();
    }

    std::vector<const char*> HelloTriangleApplication::GetRequiredExtensions()
    {
        uint sdlExtCount = 0;
        auto* sdlExts = SDL_Vulkan_GetInstanceExtensions(&sdlExtCount);

        auto exts = std::vector<const char*>(sdlExts, sdlExts + sdlExtCount);
        if (ENABLE_VALIDATION_LAYERS)
        {
            exts.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return exts;
    }

    bool HelloTriangleApplication::CheckValidationLayerSupport() const
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

    bool HelloTriangleApplication::CheckDeviceExtensionSupport(VkPhysicalDevice device)
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

    bool HelloTriangleApplication::IsDeviceSuitable(VkPhysicalDevice device)
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
            isSwapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
        }

        return FindQueueFamilies(device).IsComplete() && isExtSupported && isSwapChainAdequate;
    }

    void HelloTriangleApplication::InitializeWindow()
    {
        // Initialize SDL.
        Log("Initializing SDL...");
        bool sdlStatus = SDL_Init(SDL_INIT_VIDEO);
        Assert(sdlStatus, "Failed to initialize SDL.");

        // Create window.
        _window = SDL_CreateWindow("Hello Triangle", WIDTH, HEIGHT, SDL_WINDOW_VULKAN);
        Assert(_window != nullptr, "Failed to create window.");

        Log("Initialization complete.");
    }

    void HelloTriangleApplication::InitializeVulkan()
    {
        CreateInstance();
        SetupDebugMessenger();
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
        CreateSwapChain();
    }

    void HelloTriangleApplication::PickPhysicalDevice()
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

    VkSurfaceFormatKHR HelloTriangleApplication::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
    {
        for (const auto& availableFormat : availableFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR HelloTriangleApplication::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
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

    VkExtent2D HelloTriangleApplication::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
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

    QueueFamilyIndices HelloTriangleApplication::FindQueueFamilies(VkPhysicalDevice device)
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

    SwapChainSupportDetails HelloTriangleApplication::QuerySwapChainSupport(VkPhysicalDevice device)
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

    void HelloTriangleApplication::Cleanup()
    {
        // Deinitialize Vulkan.
        vkDestroySwapchainKHR(_device, _swapChain, nullptr);
        vkDestroyDevice(_device, nullptr);

        if (ENABLE_VALIDATION_LAYERS)
        {
            DestroyDebugUtilsMessengerExt(_instance, _debugMessenger, nullptr);
        }

        vkDestroySurfaceKHR(_instance, _surface, nullptr);
        vkDestroyInstance(_instance, nullptr);

        // Deinitialize SDL.
        Log("Deinitializing SDL...");
        SDL_DestroyWindow(_window);
        SDL_Quit();

        Log("Deinitialization complete.");
    }

    void HelloTriangleApplication::MainLoop()
    {
        while (_isRunning)
        {
            SDL_PollEvent(&_event);
            _isRunning = _event.type != SDL_EVENT_QUIT;
        }
    }

    void HelloTriangleApplication::CreateInstance()
    {
        if (ENABLE_VALIDATION_LAYERS && !CheckValidationLayerSupport())
        {
            throw std::runtime_error("validation layers requested but unavailable.");
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
        createInfo.enabledExtensionCount = sdlExts.size();
        createInfo.ppEnabledExtensionNames = sdlExts.data();

        auto debugCreateInfo = VkDebugUtilsMessengerCreateInfoEXT{};
        if (ENABLE_VALIDATION_LAYERS)
        {
            Log("Vulkan validation layers enabled.");

            createInfo.enabledLayerCount = VALIDATION_LAYERS.size();
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
            Log(ext.extensionName);
        }

        if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Vulkan instance.");
        }
    }

    void HelloTriangleApplication::CreateLogicalDevice()
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
        createInfo.queueCreateInfoCount = queueCreateInfos.size();
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = DEVICE_EXTENSIONS.size();
        createInfo.ppEnabledExtensionNames = DEVICE_EXTENSIONS.data();

        if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create logical device.");
        }

        vkGetDeviceQueue(_device, idxs.GraphicsFamily, 0, &_graphicsQueue);
        vkGetDeviceQueue(_device, idxs.PresentFamily, 0, &_presentQueue);
    }

    void HelloTriangleApplication::CreateSurface()
    {
        if (!SDL_Vulkan_CreateSurface(_window, _instance, nullptr, &_surface))
        {
            throw std::runtime_error("Failed to create window surface.");
        }
    }

    void HelloTriangleApplication::CreateSwapChain()
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

    void HelloTriangleApplication::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
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

    void HelloTriangleApplication::SetupDebugMessenger()
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

    VKAPI_ATTR VkBool32 VKAPI_CALL HelloTriangleApplication::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
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

    Renderer g_Renderer = Renderer();

    void Renderer::Update()
    {

    }
}
