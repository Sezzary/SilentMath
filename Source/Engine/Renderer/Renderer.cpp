#include "Framework.h"
#include "Engine/Renderer/Renderer.h"

namespace Silent::Renderer
{
    bool QueueFamilyIndices::IsComplete()
    {
        return GraphicsFamily != NO_VALUE;
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

    bool HelloTriangleApplication::IsDeviceSuitable(VkPhysicalDevice device)
    {
        /*auto deviceProperties = VkPhysicalDeviceProperties{};
        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        auto deviceFeatures = VkPhysicalDeviceFeatures{};
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);*/

        // TODO: Pick best GPU.

        return FindQueueFamilies(device).IsComplete();
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
        PickPhysicalDevice();
        CreateLogicalDevice();
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
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                idxs.GraphicsFamily = i;
            }

            if (idxs.IsComplete())
            {
                break;
            }
        }

        return idxs;
    }

    void HelloTriangleApplication::Cleanup()
    {
        // Deinitialize Vulkan.
        vkDestroyDevice(_device, nullptr);

        if (ENABLE_VALIDATION_LAYERS)
        {
            DestroyDebugUtilsMessengerExt(_instance, _debugMessenger, nullptr);
        }

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
        float queuePriority = 1.0f;

        auto queueCreateInfo = VkDeviceQueueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = idxs.GraphicsFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        auto deviceFeatures = VkPhysicalDeviceFeatures{};

        auto createInfo = VkDeviceCreateInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = 0;

        if (vkCreateDevice(_physicalDevice, &createInfo, nullptr, &_device) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create logical device.");
        }

        vkGetDeviceQueue(_device, idxs.GraphicsFamily, 0, &_graphicsQueue);
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
