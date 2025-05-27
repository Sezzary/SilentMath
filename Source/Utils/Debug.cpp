#include "Framework.h"
#include "Utils/Debug.h"

#include "Engine/Application.h"
#include "Engine/Renderer/Renderer.h"
#include "Engine/Services/Time.h"
#include "Utils/Parallel.h"
#include "Utils/Utils.h"

using namespace Silent::Renderer;
using namespace Silent::Services;
using namespace Silent::Utils;

namespace Silent::Utils::Debug
{
    constexpr char LOGGER_NAME[] = "Logger";

    static auto Messages  = std::vector<std::string>{};
    static auto StartTime = std::chrono::high_resolution_clock::time_point{};

    void Scratchpad()
    {
        const auto& input = g_App.GetInput();

        //Log("A val" + std::to_string(input.GetAction(In::A).GetState()), LogLevel::Info, LogMode::Debug, true);

        if (input.GetAction(In::A).IsHeld())
        {
            if (g_Time.TestInterval(SEC_TO_TICK(0.25f)))
            {
                //Log("Holding A", LogLevel::Info, LogMode::Debug, true);
            }
        }

        if (input.GetAction(In::A).IsClicked())
        {
            Log("Clicked A", LogLevel::Info, LogMode::Debug, true);
        }

        if (input.GetAction(In::A).IsReleased())
        {
            //Log("Released A", LogLevel::Info, LogMode::Debug, true);
        }
    }

    void InitializeDebug()
    {
        constexpr char LOG_FILENAME[] = "Log.txt";

        const auto& config = g_App.GetConfig();

        // Set file and console log targets.
        auto path        = config.GetAppFolder() / LOG_FILENAME;
        auto fileSink    = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path.string(), true); // `true` = create new log at launch.
        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        auto logger      = std::make_shared<spdlog::logger>(LOGGER_NAME, spdlog::sinks_init_list{ fileSink, consoleSink });

        // Initialize logger.
        spdlog::initialize_logger(logger);
        logger->set_level(spdlog::level::info);
        logger->flush_on(spdlog::level::info);
        logger->set_pattern("[%Y-%b-%d %T] [%^%l%$] %v");

        // Initialize ImGui.
        ImGui::CreateContext();
        auto imguiPath             = (config.GetWorkFolder() / "imgui.ini").string();
        ImGui::GetIO().IniFilename = CopyString(imguiPath.c_str(), imguiPath.size());
    }

    void DeinitializeDebug()
    {
        spdlog::shutdown();
    }

    void UpdateDebug()
    {
        if constexpr (IS_DEBUG_BUILD)
        {
            Scratchpad();
        }

        const auto& options = g_App.GetConfig().GetOptions();
        if (!options.EnableDebugMode)
        {
            return;
        }

        // TODO: Print messages to ImGui window.
        Messages.clear();

        CreateGui([]()
        {
            ImGui::ShowDemoWindow();
        });

        CreateGui([]()
        {
            ImGui::Begin("My Window");
            ImGui::Text("Hello. It's me. =^.^=");
            ImGui::End();
        });
    }

    void Message(const std::string& msg, ...)
    {
        constexpr uint BUFFER_SIZE = 255;

        const auto& options = g_App.GetConfig().GetOptions();
        if (!options.EnableDebugMode)
        {
            return;
        }

        // Initialize buffer.
        char buffer[BUFFER_SIZE];
        std::memset(buffer, 0, BUFFER_SIZE);

        // Format string.
        va_list args = nullptr;
        va_start(args, msg.c_str());
        vsnprintf(buffer, BUFFER_SIZE, msg.c_str(), args);
        va_end(args);

        // LOCK: Restrict `Messages` access.
        static auto mutex = std::mutex();
        {
            auto lock = std::lock_guard(mutex);
            Messages.push_back(buffer);
        }
    }

    void Log(const std::string& msg, LogLevel level, LogMode mode, bool repeat)
    {
        // Ignore debug mode messages in release mode.
        if constexpr (!IS_DEBUG_BUILD)
        {
            if (mode == LogMode::Debug)
            {
                return;
            }
        }

        // LOCK: Restrict logger access.
        static auto mutex = std::mutex();
        {
            auto lock = std::lock_guard(mutex);

            static auto prevMsg = std::string();
            if (prevMsg == msg && !repeat)
            {
                return;
            }
            prevMsg = msg;

            // Get logger instance.
            auto logger = spdlog::get(LOGGER_NAME);
            if (!logger)
            {
                return;
            }

            // Log message.
            switch (level)
            {
                default:
                case LogLevel::Info:
                {
                    logger->info(msg);
                    break;
                }
                
                case LogLevel::Warning:
                {
                    logger->warn(msg);
                    break;
                }

                case LogLevel::Error:
                {
                    logger->error(msg);
                    break;
                }
            }
            logger->flush();
        }
    }

    void Assert(bool cond, const std::string& msg)
    {
        if constexpr (IS_DEBUG_BUILD)
        {
            if (!cond)
            {
                throw std::runtime_error("Assertion failed. " + msg);
            }
        }
    };

    void StartTimer()
    {
        if constexpr (IS_DEBUG_BUILD)
        {
            StartTime = std::chrono::high_resolution_clock::now();
        }
    }

    void EndTimer()
    {
        if constexpr (IS_DEBUG_BUILD)
        {
            auto endTime  = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - StartTime);
            Message("Execution (μs): %d", duration.count());
        }
    }

    void CreateGui(std::function<void()> drawFunc)
    {
        auto& renderer = g_App.GetRenderer();
        renderer.SubmitDebugGui(drawFunc);
    }

    void CreateLine(const Vector3& from, const Vector3& to, const Color& color, DebugPage page)
    {
        auto& renderer = g_App.GetRenderer();
        renderer.SubmitDebugLine(from, to, color, page);
    }

    void CreateTriangle(const Vector3& vert0, const Vector3& vert1, const Vector3& vert2, const Color& color, DebugPage page)
    {
        auto& renderer = g_App.GetRenderer();
        renderer.SubmitDebugTriangle(vert0, vert1, vert2, color, page);
    }

    void CreateTarget(const Vector3& center, const Quaternion& rot, float radius, const Color& color, DebugPage page)
    {
        auto& renderer = g_App.GetRenderer();
        renderer.SubmitDebugTarget(center, rot, radius, color, page);
    }

    void CreateBox(const OrientedBoundingBox& obb, const Color& color, bool isWireframe, DebugPage page)
    {
        auto& renderer = g_App.GetRenderer();
        renderer.SubmitDebugBox(obb, color, isWireframe, page);
    }

    void CreateSphere(const BoundingSphere& sphere, const Color& color, bool isWireframe, DebugPage page)
    {
        auto& renderer = g_App.GetRenderer();
        renderer.SubmitDebugSphere(sphere, color, isWireframe, page);
    }

    void CreateCylinder(const Vector3& center, const Quaternion& rot, float radius, float length, const Color& color, bool isWireframe, DebugPage page)
    {
        auto& renderer = g_App.GetRenderer();
        renderer.SubmitDebugCylinder(center, rot, radius, length, color, isWireframe, page);
    }

    void CreateCone(const Vector3& center, const Quaternion& rot, float radius, float length, const Color& color, bool isWireframe, DebugPage page)
    {
        auto& renderer = g_App.GetRenderer();
        renderer.SubmitDebugCone(center, rot, radius, length, color, isWireframe, page);
    }
}
