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

        VkInstance               _instance       = nullptr;
        VkDebugUtilsMessengerEXT _debugMessenger = nullptr;
        VkSurfaceKHR             _surface        = nullptr;

        VkPhysicalDevice         _physicalDevice = VK_NULL_HANDLE;
        VkDevice                 _device         = nullptr;

        VkQueue                  _graphicsQueue  = nullptr;
        VkQueue                  _presentQueue   = nullptr;

        VkSwapchainKHR             _swapChain             = nullptr;
        VkFormat                   _swapChainImageFormat  = {};
        VkExtent2D                 _swapChainExtent       = {};
        std::vector<VkImage>       _swapChainImages       = {};
        std::vector<VkImageView>   _swapChainImageViews   = {};
        std::vector<VkFramebuffer> _swapChainFramebuffers = {};

        VkRenderPass             _renderPass       = nullptr;
        VkPipelineLayout         _pipelineLayout   = nullptr;
        VkPipeline               _graphicsPipeline = nullptr;

        VkCommandPool            _commandPool   = nullptr;
        VkCommandBuffer          _commandBuffer = nullptr;

        VkSemaphore _imageAvailableSemaphore = nullptr;
        VkSemaphore _renderFinishedSemaphore = nullptr;
        VkFence     _inFlightFence           = nullptr;

        bool                     _isRunning     = true;

        void Run();

    private:
        // Getters

        std::vector<const char*> GetRequiredExtensions();

        // Inquirers

        bool CheckValidationLayerSupport() const;
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
        bool IsDeviceSuitable(VkPhysicalDevice device);

        // Utilities

        void InitializeWindow();
        void InitializeVulkan();
        void Cleanup();

        void                    PickPhysicalDevice();
        VkSurfaceFormatKHR      ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR        ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D              ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        QueueFamilyIndices      FindQueueFamilies(VkPhysicalDevice device);
        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

        void MainLoop();
        void DrawFrame();

        void           CreateInstance();
        void           CreateLogicalDevice();
        void           CreateSurface();
        void           CreateSwapChain();
        void           CreateImageViews();
        void           CreateRenderPass();
        void           CreateGraphicsPipeline();
        void           CreateFramebuffers();
        void           CreateCommandPool();
        void           CreateCommandBuffer();
        void           CreateSyncObjects();
        VkShaderModule CreateShaderModule(const std::vector<char>& code);

        void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32 imageIdx);

        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void SetupDebugMessenger();

        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                            void* pUserData);
    };

    VkResult          CreateDebugUtilsMessengerExt(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                   const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void              DestroyDebugUtilsMessengerExt(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    std::vector<char> ReadFile(const std::string& fileName);

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
