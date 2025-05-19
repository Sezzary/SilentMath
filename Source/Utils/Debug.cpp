#include "Framework.h"
#include "Utils/Debug.h"

#include "Engine/Application.h"
#include "Engine/Renderer/Renderer.h"

using namespace Silent::Renderer;

namespace Silent::Utils::Debug
{
    static auto Messages  = std::vector<std::string>{};
    static auto StartTime = std::chrono::high_resolution_clock::time_point{};

    void InitializeDebug()
    {
        if constexpr (IS_DEBUG)
        {
            // TODO: Save log to text file.
        }     
    }

    void UpdateDebug()
    {
        if constexpr (IS_DEBUG)
        {
            // TODO: Print messages to ImGui window.

            Messages.clear();
        }
    }

    void Log(const std::string& msg, LogLevel level, bool allowRepeat)
    {
        // LOCK: Restrict previous message access.
        static auto mutex = std::mutex();
        {
            auto lock = std::lock_guard(mutex);

            static auto prevMsg = std::string();
            if (prevMsg == msg && !allowRepeat)
            {
                return;
            }
            prevMsg = msg;
        }

        switch (level)
        {
            default:
            case LogLevel::Info:
            {
                spdlog::info(msg);
                break;
            }
            
            case LogLevel::Warning:
            {
                spdlog::warn(msg);
                break;
            }

            case LogLevel::Error:
            {
                spdlog::error(msg);
                break;
            }
        }
    }

    void Message(const char* msg, ...)
    {
        if constexpr (IS_DEBUG)
        {
            constexpr int BUFFER_SIZE = 255;

            // Initialize buffer.
            char buffer[BUFFER_SIZE];
            std::memset(buffer, 0, BUFFER_SIZE);

            // Format string.
            va_list args;
            va_start(args, msg);
            vsnprintf(buffer, BUFFER_SIZE, msg, args);
            va_end(args);

            // LOCK: Restrict `Messages` access.
            static auto mutex = std::mutex();
            {
                auto lock = std::lock_guard(mutex);
                Messages.push_back(buffer);
            }
        }
    }

    void Assert(bool cond, const std::string& msg)
    {
        if constexpr (IS_DEBUG)
        {
            if (cond)
            {
                return;
            }

            Log(msg, LogLevel::Error);
            throw std::runtime_error("Assertion failed.");
        }
    };

    void StartTimer()
    {
        if constexpr (IS_DEBUG)
        {
            StartTime = std::chrono::high_resolution_clock::now();
        }
    }

    void EndTimer()
    {
        if constexpr (IS_DEBUG)
        {
            auto endTime  = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - StartTime);
            Message("Execution (μs): %d", duration.count());
        }
    }

    void CreateGui(std::function<void()> drawFunc)
    {
        if constexpr (IS_DEBUG)
        {
            auto& renderer = g_App.GetRenderer();

            renderer.SubmitGui(drawFunc);
        }
    }

    /*void CreateLine(const Vector3& from, const Vector3& to, const Color& color, DebugPage page)
    {
        if constexpr (IS_DEBUG)
        {
           auto& renderer = g_App.GetRenderer();

           renderer.CreateLine(from, to, color, page);
        }
    }

    void CreateTriangle(const Vector3& vert0, const Vector3& vert1, const Vector3& vert2, const Color& color, DebugPage page)
    {
        if constexpr (IS_DEBUG)
        {
           auto& renderer = g_App.GetRenderer();

           renderer.CreateTriangle(vert0, vert1, vert2, color, page);
        }
    }

    void CreateTarget(const Vector3& center, const Quaternion& rot, float radius, const Color& color, DebugPage page)
    {
        if constexpr (IS_DEBUG)
        {
           auto& renderer = g_App.GetRenderer();

           renderer.CreateTarget(center, rot, radius, color, page);
        }
    }

    void CreateBox(const AxisAlignedBoundingBox& aabb, const Color& color, bool isWireframe, DebugPage page)
    {
        if constexpr (IS_DEBUG)
        {
           auto& renderer = g_App.GetRenderer();

           renderer.CreateBox(aabb, color, isWireframe, page);
        }
    }

    void CreateBox(const OrientedBoundingBox& obb, const Color& color, bool isWireframe, DebugPage page)
    {
        if constexpr (IS_DEBUG)
        {
           auto& renderer = g_App.GetRenderer();

           renderer.CreateBox(obb, color, isWireframe, page);
        }
    }

    void CreateSphere(const BoundingSphere& sphere, const Color& color, bool isWireframe, DebugPage page)
    {
        if constexpr (IS_DEBUG)
        {
           auto& renderer = g_App.GetRenderer();

           renderer.CreateSphere(sphere, color, isWireframe, page);
        }
    }

    void CreateCylinder(const Vector3& center, const Quaternion& rot, float radius, float length, const Color& color, bool isWireframe, DebugPage page)
    {
        if constexpr (IS_DEBUG)
        {
           auto& renderer = g_App.GetRenderer();

           renderer.CreateCylinder(center, rot, radius, length, color, isWireframe, page);
        }
    }

    void CreateCone(const Vector3& center, const Quaternion& rot, float radius, float length, const Color& color, bool isWireframe, DebugPage page)
    {
        if constexpr (IS_DEBUG)
        {
           auto& renderer = g_App.GetRenderer();

           renderer.CreateCone(center, rot, radius, length, color, isWireframe, page);
        }
    }*/
}
