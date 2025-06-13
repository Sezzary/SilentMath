#include "Framework.h"
#include "Utils/Debug.h"

#include "Engine/Application.h"
#include "Engine/Input/Input.h"
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

    DebugData g_DebugData = {};

    static auto Messages  = std::vector<std::string>{};
    static auto StartTime = std::chrono::high_resolution_clock::time_point{};

    void Scratchpad()
    {
        
    }

    void InitializeDebug()
    {
        constexpr char LOG_FILENAME[]   = "Log.txt";
        constexpr char IMGUI_FILENAME[] = "imgui.ini";

        const auto& fs = g_App.GetFilesystem();

        // Set file and console log targets.
        auto path        = fs.GetAppFolder() / LOG_FILENAME;
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
        auto imguiPath             = (fs.GetWorkFolder() / IMGUI_FILENAME).string();
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
            // Handle scratchpad.
            Scratchpad();
        }

        // If debug GUI is disabled, return early.
        const auto& options = g_App.GetOptions();
        if (!options->EnableDebugGui)
        {
            Messages.clear();
            return;
        }

        CreateGui([]()
        {
            ImGui::ShowDemoWindow();
        });

        // Create debug GUI.
        CreateGui([]()
        {
            constexpr const char* FRAME_RATE_ITEMS[]   = { "30", "60", "120", "Unlimited" };
            constexpr const char* RENDER_SCALE_ITEMS[] = { "Native", "Half", "Quarter", "Retro" };
            constexpr const char* ASPECT_RATIO_ITEMS[] = { "Native", "Widescreen", "Retro" };
            constexpr const char* TEX_FILTER_ITEMS[]   = { "Nearest", "Bilinear" };
            constexpr const char* LIGHTING_ITEMS[]     = { "Per vertex", "Per pixel" };
            constexpr const char* SOUND_ITEMS[]        = { "Stereo", "Monaural" };
            constexpr const char* BLOOD_COLOR_ITEMS[]  = { "Normal", "Green", "Violet", "Black" };

            auto& options  = g_App.GetOptions();
            auto& renderer = g_App.GetRenderer();

            // Main tabs.
            if (ImGui::BeginTabBar("MainTabs", ImGuiTabBarFlags_FittingPolicyScroll))
            {
                if constexpr (IS_DEBUG_BUILD)
                {
                    // `Scratchpad` tab.
                    if (ImGui::BeginTabItem("Scratchpad"))
                    {
                        // 'Alpha blend' slider.
                        ImGui::SliderFloat("Alpha Blend", &g_DebugData.BlendAlpha, 0.0f, 1.0f);
    
                        ImGui::EndTabItem();
                    }
                }

                // `Renderer` tab.
                if (ImGui::BeginTabItem("Renderer"))
                {
                    g_DebugData.Page = DebugPage::Renderer;

                    // `Wireframe mode` checkbox.
                    ImGui::Checkbox("Wireframe mode", &g_DebugData.EnableWireframeMode);

                    ImGui::EndTabItem();
                }
                
                // `Input` tab.
                if (ImGui::BeginTabItem("Input"))
                {
                    g_DebugData.Page = DebugPage::Input;

                    const auto& input = g_App.GetInput();

                    // `Status` section.
                    ImGui::SeparatorText("Status");
                    {
                        if (ImGui::BeginTable("Status", 2))
                        {
                            // `Gamepad` enabled/disabled info.
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("Gamepad:", 0, 0);
                            ImGui::TableSetColumnIndex(1);
                            input.IsGamepadConnected() ? ImGui::Text("Connected", 0, 1) : ImGui::Text("Not connected", 0, 1);
                            
                            // `Cursor` info.
                            const auto& cursorPos = input.GetCursorPosition();
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("Cursor:", 1, 0);
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("(%.2f, %.2f)", cursorPos.x, cursorPos.y, 1, 1);

                            ImGui::EndTable();
                        }
                    }

                    // `Analog Axes` section.
                    ImGui::SeparatorText("Analog Axes");
                    {
                        if (ImGui::BeginTable("Analog Axes", 2))
                        {
                            // `Move axis` info.
                            const auto& moveAxis = input.GetAnalogAxis(AnalogAxisId::Move);
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("Move axis:", 0, 0);
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("(%.2f, %.2f)", moveAxis.x, moveAxis.y, 0, 1);

                            // `Camera axis` info.
                            const auto& camAxis = input.GetAnalogAxis(AnalogAxisId::Camera);
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("Camera axis:", 1, 0);
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("(%.2f, %.2f)", camAxis.x, camAxis.y, 1, 1);

                            // `Mouse axis` info.
                            const auto& mouseAxis = input.GetAnalogAxis(AnalogAxisId::Mouse);
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("Mouse axis:", 2, 0);
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("(%.2f, %.2f)", mouseAxis.x, mouseAxis.y, 2, 1);

                            // `Left stick axis` info.
                            const auto& leftStickAxis = input.GetAnalogAxis(AnalogAxisId::StickLeft);
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("Left stick axis:", 3, 0);
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("(%.2f, %.2f)", leftStickAxis.x, leftStickAxis.y, 3, 1);

                            // `Right stick axis` info.
                            const auto& rightStickAxis = input.GetAnalogAxis(AnalogAxisId::StickRight);
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("Right stick axis:", 4, 0);
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("(%.2f, %.2f)", rightStickAxis.x, rightStickAxis.y, 4, 1);

                            ImGui::EndTable();
                        }
                    }

                    ImGui::EndTabItem();
                }

                // `Cheats` tab.
                if (ImGui::BeginTabItem("Cheats"))
                {
                    g_DebugData.Page = DebugPage::Cheats;

                    ImGui::EndTabItem();
                }

                // `Options` tab.
                if (ImGui::BeginTabItem("Options"))
                {
                    g_DebugData.Page  = DebugPage::Options;
                    bool isOptChanged = false;

                    // `Graphics` section.
                    ImGui::SeparatorText("Graphics");
                    {
                        // `Reset` button.
                        if (ImGui::Button("Reset graphics"))
                        {
                            options.SetDefaultGraphicsOptions();
                        }

                        // `Enable fullscreen` checkbox.
                        if (ImGui::Checkbox("Enable fullscreen", &options->EnableFullscreen))
                        {
                            auto& renderer = g_App.GetRenderer();
                            renderer.SignalResize();
                            isOptChanged = true;
                        }

                        // `Brightness level` slider.
                        if (ImGui::SliderInt("Brightness level", &options->BrightnessLevel, 0, 7))
                        {
                            isOptChanged = true;
                        }

                        // `Frame rate` combo.
                        int frameRate = (int)options->FrameRate;
                        if (ImGui::Combo("Frame rate", &frameRate, FRAME_RATE_ITEMS, IM_ARRAYSIZE(FRAME_RATE_ITEMS)))
                        {
                            options->FrameRate = (FrameRateType)frameRate;
                            isOptChanged = true;
                        }

                        // `Render scale` combo.
                        int renderScale = (int)options->RenderScale;
                        if (ImGui::Combo("Render scale", &renderScale, RENDER_SCALE_ITEMS, IM_ARRAYSIZE(RENDER_SCALE_ITEMS)))
                        {
                            options->RenderScale = (RenderScaleType)renderScale;
                            isOptChanged = true;
                        }

                        // `Aspect ratio` combo.
                        int aspectRatio = (int)options->AspectRatio;
                        if (ImGui::Combo("Aspect ratio", &aspectRatio, ASPECT_RATIO_ITEMS, IM_ARRAYSIZE(ASPECT_RATIO_ITEMS)))
                        {
                            options->AspectRatio = (AspectRatioType)aspectRatio;
                            isOptChanged = true;
                        }

                        // `Texture filter` combo.
                        int texFilter = (int)options->TextureFilter;
                        if (ImGui::Combo("Texture filter", &texFilter, TEX_FILTER_ITEMS, IM_ARRAYSIZE(TEX_FILTER_ITEMS)))
                        {
                            options->TextureFilter = (TextureFilterType)texFilter;
                            renderer.RefreshTextureFilter();
                            isOptChanged = true;
                        }

                        // `Lighting type` combo.
                        int lighting = (int)options->Lighting;
                        if (ImGui::Combo("Lighting", &lighting, LIGHTING_ITEMS, IM_ARRAYSIZE(LIGHTING_ITEMS)))
                        {
                            options->Lighting = (LightingType)lighting;
                            isOptChanged = true;
                        }

                        // `Enable dithering` checkbox.
                        if (ImGui::Checkbox("Enable dithering", &options->EnableDithering))
                        {
                            isOptChanged = true;
                        }

                        // `Enable CRT filter` checkbox.
                        if (ImGui::Checkbox("Enable CRT filter", &options->EnableCrtFilter))
                        {
                            isOptChanged = true;
                        }

                        // `Enable vertex jitter` checkbox.
                        if (ImGui::Checkbox("Enable vertex jitter", &options->EnableVertexJitter))
                        {
                            isOptChanged = true;
                        }
                    }

                    // `Gameplay` section.
                    ImGui::SeparatorText("Gameplay");
                    {
                        // `Reset` button.
                        if (ImGui::Button("Reset gameplay"))
                        {
                            options.SetDefaultGameplayOptions();
                        }

                        // `Enable auto load` checkbox.
                        if (ImGui::Checkbox("Enable auto load", &options->EnableAutoLoad))
                        {
                            isOptChanged = true;
                        }

                        // `Enable subtitles` checkbox.
                        if (ImGui::Checkbox("Enable subtitles", &options->EnableSubtitles))
                        {
                            isOptChanged = true;
                        }

                        // `Sound type` combo.
                        int sound = (int)options->Sound;
                        if (ImGui::Combo("Sound", &sound, SOUND_ITEMS, IM_ARRAYSIZE(SOUND_ITEMS)))
                        {
                            options->Sound = (SoundType)sound;
                            isOptChanged = true;
                        }

                        // `Music volume` slider.
                        if (ImGui::SliderInt("Music volume", &options->BgmVolume, 0, SOUND_VOLUME_MAX))
                        {
                            isOptChanged = true;
                        }

                        // `SFX volume` slider.
                        if (ImGui::SliderInt("SFX volume", &options->SeVolume, 0, SOUND_VOLUME_MAX))
                        {
                            isOptChanged = true;
                        }

                        // `Blood color type` combo.
                        int bloodColor = (int)options->BloodColor;
                        if (ImGui::Combo("Blood color", &bloodColor, BLOOD_COLOR_ITEMS, IM_ARRAYSIZE(BLOOD_COLOR_ITEMS)))
                        {
                            options->BloodColor = (BloodColorType)bloodColor;
                            isOptChanged        = true;
                        }

                        // `Bullet adjust` slider.
                        if (ImGui::SliderInt("Bullet adjust", &options->BulletAdjust, BULLET_ADJUST_MIN, BULLET_ADJUST_MAX))
                        {
                            isOptChanged = true;
                        }
                    }

                    // `Input` section.
                    ImGui::SeparatorText("Input");
                    {
                        // `Reset` button.
                        if (ImGui::Button("Reset input"))
                        {
                            options.SetDefaultInputControlsOptions();
                        }

                        // `Enable vibration` checkbox.
                        if (ImGui::Checkbox("Enable vibration", &options->EnableVibration))
                        {
                            isOptChanged = true;
                        }

                        // `Mouse sensitivity` slider.
                        if (ImGui::SliderInt("Mouse sensitivity", &options->MouseSensitivity, 1, MOUSE_SENSITIVITY_MAX))
                        {
                            isOptChanged = true;
                        }
                        
                        // TODO
                        int  WeaponControl     = 0;
                        int  ViewControl       = 0;
                        int  RetreatTurn       = 0;
                        int  WalkRunControl    = 0;

                        // `Disable auto aiming` checkbox.
                        if (ImGui::Checkbox("Disable auto aiming", &options->DisableAutoAiming))
                        {
                            isOptChanged = true;
                        }

                        // TODO
                        int  ViewMode          = 0;
                    }

                    // `System` section.
                    ImGui::SeparatorText("System");
                    {
                        // `Reset` button.
                        if (ImGui::Button("Reset system"))
                        {
                            options.SetDefaultSystemOptions();
                        }

                        // `Enable toasts` checkbox.
                        if (ImGui::Checkbox("Enable toasts", &options->EnableToasts))
                        {
                            isOptChanged = true;
                        }

                        // `Enable parallelism` checkbox.
                        if (ImGui::Checkbox("Enable parallelism", &options->EnableParallelism))
                        {
                            isOptChanged = true;
                        }
                    }

                    // If changed, save options.
                    if (isOptChanged)
                    {
                        options.Save();
                    }

                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }

            // `Messages` section.
            ImGui::SeparatorText("Messages");
            {
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
                if (ImGui::BeginChild("MessageBox", ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing() * 8),
                    ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY))
                {
                    for (const auto& msg : Messages)
                    {
                        ImGui::Bullet();
                        ImGui::TextWrapped(msg.c_str());
                    }
                }
                ImGui::PopStyleColor();
                ImGui::EndChild();
            }
        });

        Messages.clear();
    }

    void Message(const std::string& msg, ...)
    {
        constexpr uint BUFFER_SIZE = 255;

        const auto& options = g_App.GetOptions();
        if (!options->EnableDebugGui)
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

            // Add message.
            Messages.push_back(buffer);
        }
    }

    void Log(const std::string& msg, LogLevel level, LogMode mode, bool repeat)
    {
        // Ignore debug mode messages in release build.
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
