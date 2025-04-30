#pragma once

namespace Silent::Renderer
{
    struct QueueFamilyIndices
    {
        uint GraphicsFamily = (uint)NO_VALUE;
        uint PresentFamily  = (uint)NO_VALUE;

        bool IsComplete();
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR        Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats      = {};
        std::vector<VkPresentModeKHR>   PresentModes = {};
    };

    class HelloTriangleApplication
    {
    public:
#ifdef _DEBUG
        static constexpr bool ENABLE_VALIDATION_LAYERS = true;
#else
        static constexpr bool ENABLE_VALIDATION_LAYERS = false;
#endif

        static constexpr int WIDTH  = 800;
        static constexpr int HEIGHT = 600;

        static constexpr std::array<const char*, 1> VALIDATION_LAYERS =
        {
            "VK_LAYER_KHRONOS_validation"
        };

        static constexpr std::array<const char*, 1> DEVICE_EXTENSIONS =
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        static constexpr char WINDOW_NAME[] = "Hello Triangle";

        SDL_Window*              _window         = nullptr;
        SDL_Event                _event          = {};

        VkInstance               _instance;
        VkDebugUtilsMessengerEXT _debugMessenger;
        VkSurfaceKHR             _surface;

        VkPhysicalDevice         _physicalDevice = VK_NULL_HANDLE;
        VkDevice                 _device;

        VkQueue                  _graphicsQueue;
        VkQueue                  _presentQueue;

        VkSwapchainKHR           _swapChain;
        std::vector<VkImage>     _swapChainImages      = {};
        VkFormat                 _swapChainImageFormat;
        VkExtent2D               _swapChainExtent;


        bool                     _isRunning      = true;

        void Run();

    private:
        // Getters

        std::vector<const char*> GetRequiredExtensions();

        // Inquirers

        bool CheckValidationLayerSupport() const;
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
        bool IsDeviceSuitable(VkPhysicalDevice device);

        // Utilities

        void                    InitializeWindow();
        void                    InitializeVulkan();
        void                    PickPhysicalDevice();
        VkSurfaceFormatKHR      ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR        ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D              ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        QueueFamilyIndices      FindQueueFamilies(VkPhysicalDevice device);
        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
        void                    Cleanup();

        void MainLoop();
        void CreateInstance();
        void CreateLogicalDevice();
        void CreateSurface();
        void CreateSwapChain();

        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void SetupDebugMessenger();

        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                            void* pUserData);
    };

    VkResult CreateDebugUtilsMessengerExt(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void     DestroyDebugUtilsMessengerExt(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

    class Renderer
    {
    public:
        // Constructors

        Renderer() = default;

        // Utilities

        void Update();
    };

    extern Renderer g_Renderer;
}
