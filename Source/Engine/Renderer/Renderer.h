#pragma once

namespace Silent::Renderer
{
    struct QueueFamilyIndices
    {
        uint GraphicsFamily = (uint)NO_VALUE;

        bool IsComplete();
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

        static constexpr char WINDOW_NAME[] = "Hello Triangle";

        SDL_Window*              _window         = nullptr;
        SDL_Event                _event          = {};
        VkInstance               _instance;
        VkPhysicalDevice         _physicalDevice = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT _debugMessenger;
        bool                     _isRunning      = true;

        void Run();

    private:
        // Getters

        std::vector<const char*> GetRequiredExtensions();

        // Inquirers

        bool CheckValidationLayerSupport() const;
        bool IsDeviceSuitable(VkPhysicalDevice device) const;

        // Utilities

        void               InitializeWindow();
        void               InitializeVulkan();
        void               PickPhysicalDevice();
        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
        void               Cleanup();

        void MainLoop();
        void CreateInstance();

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
