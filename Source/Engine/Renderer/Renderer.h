#pragma once

namespace Silent::Renderer
{
    enum class DebugPage
    {
        None
    };

    struct QueueFamilyIndices
    {
        uint GraphicsFamily = (uint)NO_VALUE;
        uint PresentFamily  = (uint)NO_VALUE;

        bool IsComplete();
    };

    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR        Capabilities = {};
        std::vector<VkSurfaceFormatKHR> Formats      = {};
        std::vector<VkPresentModeKHR>   PresentModes = {};
    };

    class RendererManager
    {
    private:
#ifdef _DEBUG
        static constexpr bool ENABLE_VALIDATION_LAYERS = true;
#else
        static constexpr bool ENABLE_VALIDATION_LAYERS = false;
#endif

        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;
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

        VkCommandPool                _commandPool    = nullptr;
        std::vector<VkCommandBuffer> _commandBuffers = {};

        std::vector<VkSemaphore> _imageAvailableSemaphores = {};
        std::vector<VkSemaphore> _renderFinishedSemaphores = {};
        std::vector<VkFence>     _inFlightFences           = {};

        bool _isFramebufferResized = false;

        VkDescriptorPool _descPool = nullptr;

        uint32 _activeFrame = 0;

        std::vector<std::function<void()>> _guiDrawCalls;

    public:
        // Constructors

        RendererManager() = default;

        // Utilities

        void Initialize(SDL_Window& window);
        void Deinitialize();
        void Update();
        
        void SignalResizedFramebuffer();
        void SubmitGui(std::function<void()> drawFunc);

    private:
        // Getters

        VkInstance               GetInstance();
        std::vector<const char*> GetRequiredExtensions();

        // Inquirers

        bool CheckValidationLayerSupport() const;
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
        bool IsDeviceSuitable(VkPhysicalDevice device);

        // Utilities

        void InitializeImGui();
        void DrawFrame();
        void DrawGui();

        void                    PickPhysicalDevice();
        VkSurfaceFormatKHR      ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        VkPresentModeKHR        ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        VkExtent2D              ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
        QueueFamilyIndices      FindQueueFamilies(VkPhysicalDevice device);
        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

        void           CreateInstance();
        void           CreateLogicalDevice();
        void           CreateSurface();
        void           CreateSwapChain();
        void           RecreateSwapChain();
        void           CreateImageViews();
        void           CreateRenderPass();
        void           CreateGraphicsPipeline();
        void           CreateFramebuffers();
        void           CreateCommandPool();
        void           CreateCommandBuffers();
        void           CreateSyncObjects();
        VkShaderModule CreateShaderModule(const std::vector<char>& code);

        void CleanupSwapChain();

        void            RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32 imageIdx);
        void            PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        void            SetupDebugMessenger();
        VkCommandBuffer BeginSingleTimeCommands();
        void            EndSingleTimeCommands(VkCommandBuffer commandBuffer);

        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                            void* pUserData);
    };

    extern RendererManager g_Renderer;

    VkResult          CreateDebugUtilsMessengerExt(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                   const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void              DestroyDebugUtilsMessengerExt(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    std::vector<char> ReadFile(const std::string& fileName);
}
