#include "Framework.h"
#include "Debug/PowerMenu.h"

#include "Application.h"
#include "Assets/AssetStreamer.h"
#include "Debug/Debug.h"
#include "Debug/GameData.h"
#include "Services/Options.h"
#include "Utils/Bitfield.h"
#include "Utils/Translator.h"

namespace Silent::Debug
{
    void CreatePowerMenu()
    {
        CreateGui([]()
        {
            constexpr const char* FRAME_RATE_ITEMS[]        = { "30 FPS", "60 FPS" };
            constexpr const char* RENDER_SCALE_ITEMS[]      = { "Original", "DoubleOriginal", "Native" };
            constexpr const char* ASPECT_RATIO_ITEMS[]      = { "4:3", "16:9", "Native"  };
            constexpr const char* TEX_FILTER_ITEMS[]        = { "Nearest", "Linear" };
            constexpr const char* TEXT_QUALITY_ITEMS[]      = { "Original", "Smooth" };
            constexpr const char* LIGHTING_ITEMS[]          = { "Per vertex", "Per pixel" };
            constexpr const char* SOUND_ITEMS[]             = { "Stereo", "Monaural" };
            constexpr const char* BLOOD_COLOR_ITEMS[]       = { "Normal", "Green", "Violet", "Black" };
            constexpr const char* CONTROL_INVERSION_ITEMS[] = { "Normal", "Reverse" };
            constexpr const char* WEAPON_CONTROL_ITEMS[]    = { "Switch", "Press" };
            constexpr const char* VIEW_MODE_ITEMS[]         = { "Normal", "Self view" };
            constexpr const char* PAPER_MAP_ITEMS[]         = { "Original", "Scalable" };
            constexpr const char* DIALOG_PAUSE_ITEMS[]      = { "Original", "Condensed" };

            const auto& assets     = g_App.GetAssets();
            auto&       options    = g_App.GetOptions();
            auto&       renderer   = g_App.GetRenderer();
            auto&       translator = g_App.GetTranslator();

            ImGui::Begin("Power Menu");

            // Main tabs.
            if (ImGui::BeginTabBar("MainTabs", ImGuiTabBarFlags_FittingPolicyScroll))
            {
                if constexpr (IS_DEBUG_BUILD)
                {
                    // `General` tab.
                    if (ImGui::BeginTabItem("General"))
                    {
                        if (ImGui::BeginTable("Status", 2))
                        {
                            // `Sys state` info.
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("Sys state:", 0, 0);
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%d", *g_SysState, 0, 1);

                            // `Sys state steps` info.
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("Sys state steps:", 1, 0);
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%d, %d, %d",
                                        g_SysStateSteps[0], g_SysStateSteps[1], g_SysStateSteps[2],
                                        1, 1);

                            // `Sys counters` info.
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("Sys counters:", 2, 0);
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%d, %d, %d",
                                        g_SysCounters[0], g_SysCounters[1], g_SysCounters[2],
                                        2, 1);

                            // `Game state` info.
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("Game state:", 3, 0);
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%d", *g_GameState, 3, 1);

                            // `Game state steps` info.
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("Game state steps:", 4, 0);
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%d, %d, %d",
                                        g_GameStateSteps[0], g_GameStateSteps[1], g_GameStateSteps[2],
                                        4, 1);

                            ImGui::EndTable();
                        }

                        // 'Alpha blend' slider.
                        ImGui::SliderFloat("Alpha Blend", &g_Work.BlendAlpha, 0.0f, 1.0f);
    
                        ImGui::EndTabItem();
                    }
                }

                // `Renderer` tab.
                if (ImGui::BeginTabItem("Renderer"))
                {
                    g_Work.Page = Page::Renderer;

                    // `Status` section.
                    ImGui::SeparatorText("Status");
                    {
                        if (ImGui::BeginTable("Status", 2))
                        {
                            // `FPS` info.
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("FPS:", 0, 0);
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%.2f", g_Work.Fps, 0, 1);

                            // `Frame time` info.
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("Frame time (microsec):", 1, 0);
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%d", g_Work.FrameTime, 1, 1);

                            // `Draw calls` info.
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("Draw calls:", 2, 0);
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("%d", renderer.GetDrawCallCount(), 2, 1);

                            ImGui::EndTable();
                        }
                    }

                    // `Wireframe mode` checkbox.
                    ImGui::Checkbox("Wireframe mode", &g_Work.EnableWireframeMode);

                    ImGui::EndTabItem();
                }

                // `Resources` tab.
                if (ImGui::BeginTabItem("Resources"))
                {
                    // `Loaded Assets` section.
                    ImGui::SeparatorText("Loaded Assets");
                    {
                        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
                        if (ImGui::BeginChild("LoadedAssetsBox",
                                              ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing() * 8),
                                              ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY))
                        {
                            auto assetNames = assets.GetLoadedNames();
                            for (const auto& assetName : assetNames)
                            {
                                ImGui::Bullet();
                                ImGui::TextWrapped(assetName.c_str());
                            }
                        }
                        ImGui::PopStyleColor();
                        ImGui::EndChild();
                    }

                    ImGui::EndTabItem();
                }

                // `Input` tab.
                if (ImGui::BeginTabItem("Input"))
                {
                    g_Work.Page = Page::Input;

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

                    // `Actions` section.
                    /*ImGui::SeparatorText("Actions");
                    {
                        const auto& input = g_App.GetInput();

                        // Collect action statuses.
                        int  flagCount           = (int)ACTION_ID_GROUPS.at(USER_ACTION_GROUP_IDS.back()).back();
                        auto clickedActionFlags  = Bitfield(flagCount);
                        auto heldActionFlags     = Bitfield(flagCount);
                        auto releasedActionFlags = Bitfield(flagCount);
                        for (int i = 0; i < flagCount; i++)
                        {
                            auto actionId = (ActionId)i;
                            const auto& action = input.GetAction(actionId);

                            if (action.IsClicked())
                            {
                                clickedActionFlags.Set(i);
                            }
                            if (action.IsHeld())
                            {
                                heldActionFlags.Set(i);
                            }
                            if (action.IsReleased())
                            {
                                releasedActionFlags.Set(i);
                            }
                        }

                        // @todo Using `c_str` crashes.

                        // `Clicked` info.
                        /*ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Clicked:", 0, 0);
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text(clickedActionFlags.ToString().c_str(), 0, 1);*/

                        // `Held` info.
                        /*ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Held:", 1, 0);
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text(heldActionFlags.ToString().c_str(), 1, 1);*/

                        // `Released` info.
                        /*ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::Text("Released:", 2, 0);
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text(releasedActionFlags.ToString().c_str(), 2, 1);*/
                    //}*/

                    // `Analog Axes` section.
                    ImGui::SeparatorText("Analog Axes");
                    {
                        if (ImGui::BeginTable("Analog Axes", 2))
                        {
                            // `Move axis` info.
                            const auto& moveAxis = input.GetAnalogAxis(AnalogAxisId::Move);
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("Move:", 0, 0);
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("(%.2f, %.2f)", moveAxis.x, moveAxis.y, 0, 1);

                            // `Camera axis` info.
                            const auto& camAxis = input.GetAnalogAxis(AnalogAxisId::Camera);
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("Camera:", 1, 0);
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("(%.2f, %.2f)", camAxis.x, camAxis.y, 1, 1);

                            // `Mouse axis` info.
                            const auto& mouseAxis = input.GetAnalogAxis(AnalogAxisId::Mouse);
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("Mouse:", 2, 0);
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("(%.2f, %.2f)", mouseAxis.x, mouseAxis.y, 2, 1);

                            // `Left stick axis` info.
                            const auto& leftStickAxis = input.GetAnalogAxis(AnalogAxisId::StickLeft);
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("Left stick:", 3, 0);
                            ImGui::TableSetColumnIndex(1);
                            ImGui::Text("(%.2f, %.2f)", leftStickAxis.x, leftStickAxis.y, 3, 1);

                            // `Right stick axis` info.
                            const auto& rightStickAxis = input.GetAnalogAxis(AnalogAxisId::StickRight);
                            ImGui::TableNextRow();
                            ImGui::TableSetColumnIndex(0);
                            ImGui::Text("Right stick:", 4, 0);
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
                    g_Work.Page = Page::Cheats;

                    // `Freeze mode` checkbox.
                    ImGui::Checkbox("Freeze mode", &g_Work.EnableFreezeMode);

                    ImGui::EndTabItem();
                }

                // `Options` tab.
                if (ImGui::BeginTabItem("Options"))
                {
                    g_Work.Page  = Page::Options;
                    bool isOptChanged = false;

                    // `Graphics` section.
                    ImGui::SeparatorText("Graphics");
                    {
                        // `Reset` button.
                        if (ImGui::Button("Reset##0"))
                        {
                            options.SetDefaultGraphicsOptions();
                            isOptChanged = true;
                        }

                        // `Toggle fullscreen` button.
                        if (ImGui::Button("Toggle fullscreen"))
                        {
                            g_App.ToggleFullscreen();
                        }

                        // `Brightness level` slider.
                        if (ImGui::SliderInt("Brightness level", &options->BrightnessLevel, 0, BRIGHTNESS_LEVEL_MAX))
                        {
                            isOptChanged = true;
                        }

                        // `Frame rate` combo.
                        int frameRate = (int)options->FrameRate;
                        if (ImGui::Combo("Frame rate", &frameRate, FRAME_RATE_ITEMS, IM_ARRAYSIZE(FRAME_RATE_ITEMS)))
                        {
                            options->FrameRate = (FrameRateType)frameRate;
                            isOptChanged       = true;
                        }

                        // `Render scale` combo.
                        int renderScale = (int)options->RenderScale;
                        if (ImGui::Combo("Render scale", &renderScale, RENDER_SCALE_ITEMS, IM_ARRAYSIZE(RENDER_SCALE_ITEMS)))
                        {
                            options->RenderScale = (RenderScaleType)renderScale;
                            isOptChanged         = true;
                        }

                        // `Aspect ratio` combo.
                        int aspectRatio = (int)options->AspectRatio;
                        if (ImGui::Combo("Aspect ratio", &aspectRatio, ASPECT_RATIO_ITEMS, IM_ARRAYSIZE(ASPECT_RATIO_ITEMS)))
                        {
                            options->AspectRatio = (AspectRatioType)aspectRatio;
                            isOptChanged         = true;
                        }

                        // `Texture filter` combo.
                        int texFilter = (int)options->TextureFilter;
                        if (ImGui::Combo("Texture filter", &texFilter, TEX_FILTER_ITEMS, IM_ARRAYSIZE(TEX_FILTER_ITEMS)))
                        {
                            options->TextureFilter = (TextureFilterType)texFilter;
                            isOptChanged           = true;
                        }

                        // `Text quality` combo.
                        int textQuality = (int)options->TextQuality;
                        if (ImGui::Combo("Text quality", &textQuality, TEXT_QUALITY_ITEMS, IM_ARRAYSIZE(TEXT_QUALITY_ITEMS)))
                        {
                            options->TextQuality = (TextQualityType)textQuality;
                            isOptChanged         = true;
                        }

                        // `Lighting type` combo.
                        int lighting = (int)options->Lighting;
                        if (ImGui::Combo("Lighting", &lighting, LIGHTING_ITEMS, IM_ARRAYSIZE(LIGHTING_ITEMS)))
                        {
                            options->Lighting = (LightingType)lighting;
                            isOptChanged      = true;
                        }

                        // `Enable vertex jitter` checkbox.
                        if (ImGui::Checkbox("Enable vertex jitter", &options->EnableVertexJitter))
                        {
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

                        // `Enable vignette` checkbox.
                        if (ImGui::Checkbox("Enable vignette", &options->EnableVignette))
                        {
                            isOptChanged = true;
                        }
                    }

                    // `Gameplay` section.
                    ImGui::SeparatorText("Gameplay");
                    {
                        // `Reset` button.
                        if (ImGui::Button("Reset##1"))
                        {
                            options.SetDefaultGameplayOptions();
                            isOptChanged = true;
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

                        const auto& locales = translator.GetLocales();
                        int         langIdx = 0;

                        // Collect languages.
                        auto langItems = std::vector<const char*>{};
                        langItems.reserve(locales.size());
                        for (int i = 0; i < locales.size(); i++)
                        {
                            const auto& locale = locales[i];

                            langItems.push_back(locale.Name.c_str());
                            if (locale.Name == translator.GetActiveLocaleName())
                            {
                                langIdx = i;
                            }
                        }

                        // `Language` combo.
                        if (ImGui::Combo("Language", &langIdx, langItems.data(), (int)langItems.size()))
                        {
                            options->Language = locales[langIdx].Name;
                            translator.SetActiveLocale(options->Language);

                            isOptChanged = true;
                        }

                        // `Sound type` combo.
                        int sound = (int)options->Sound;
                        if (ImGui::Combo("Sound", &sound, SOUND_ITEMS, IM_ARRAYSIZE(SOUND_ITEMS)))
                        {
                            options->Sound = (SoundType)sound;
                            isOptChanged   = true;
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
                        if (ImGui::Button("Reset##2"))
                        {
                            options.SetDefaultInputControlsOptions();
                            isOptChanged = true;
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

                        // `Weapon control` combo.
                        int weaponCtrl = (int)options->WeaponControl;
                        if (ImGui::Combo("Weapon control", &weaponCtrl, WEAPON_CONTROL_ITEMS, IM_ARRAYSIZE(WEAPON_CONTROL_ITEMS)))
                        {
                            options->WeaponControl = (WeaponControlType)weaponCtrl;
                            isOptChanged           = true;
                        }

                        // `View control` combo.
                        int viewCtrl = (int)options->ViewControl;
                        if (ImGui::Combo("View control", &viewCtrl, CONTROL_INVERSION_ITEMS, IM_ARRAYSIZE(CONTROL_INVERSION_ITEMS)))
                        {
                            options->ViewControl = (ControlInversionType)viewCtrl;
                            isOptChanged           = true;
                        }

                        // `Retreat turn control` combo.
                        int retreatTurnCtrl = (int)options->RetreatTurnControl;
                        if (ImGui::Combo("Retreat turn control", &retreatTurnCtrl, CONTROL_INVERSION_ITEMS, IM_ARRAYSIZE(CONTROL_INVERSION_ITEMS)))
                        {
                            options->RetreatTurnControl = (ControlInversionType)retreatTurnCtrl;
                            isOptChanged                = true;
                        }

                        // `Walk/run control` combo.
                        int walkRunCtrl = (int)options->WalkRunControl;
                        if (ImGui::Combo("Walk/run control", &walkRunCtrl, CONTROL_INVERSION_ITEMS, IM_ARRAYSIZE(CONTROL_INVERSION_ITEMS)))
                        {
                            options->WalkRunControl = (ControlInversionType)walkRunCtrl;
                            isOptChanged            = true;
                        }

                        // `Disable auto aiming` checkbox.
                        if (ImGui::Checkbox("Disable auto aiming", &options->DisableAutoAiming))
                        {
                            isOptChanged = true;
                        }

                        // `View mode` combo.
                        int viewMode = (int)options->ViewMode;
                        if (ImGui::Combo("View mode", &viewMode, VIEW_MODE_ITEMS, IM_ARRAYSIZE(VIEW_MODE_ITEMS)))
                        {
                            options->ViewMode = (ViewMode)viewMode;
                            isOptChanged      = true;
                        }
                    }

                    // `Enhancements` section.
                    ImGui::SeparatorText("Enhancements");
                    {
                        // `Reset` button.
                        if (ImGui::Button("Reset##3"))
                        {
                            options.SetDefaultEnhancementsOptions();
                            isOptChanged = true;
                        }

                        // `Dialog pause` combo.
                        int dialogPause = (int)options->DialogPause;
                        if (ImGui::Combo("Dialog pause", &dialogPause, DIALOG_PAUSE_ITEMS, IM_ARRAYSIZE(DIALOG_PAUSE_ITEMS)))
                        {
                            options->DialogPause = (DialogPauseType)dialogPause;
                            isOptChanged         = true;
                        }

                        // `Paper map` combo.
                        int paperMap = (int)options->PaperMap;
                        if (ImGui::Combo("Paper map", &paperMap, PAPER_MAP_ITEMS, IM_ARRAYSIZE(PAPER_MAP_ITEMS)))
                        {
                            options->PaperMap = (PaperMapQuality)paperMap;
                            isOptChanged      = true;
                        }
                    }

                    // `System` section.
                    ImGui::SeparatorText("System");
                    {
                        // `Reset` button.
                        if (ImGui::Button("Reset##4"))
                        {
                            options.SetDefaultSystemOptions();
                            isOptChanged = true;
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

                    // Save options if changed.
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
                if (ImGui::BeginChild("MessageBox",
                                      ImVec2(-FLT_MIN, ImGui::GetTextLineHeightWithSpacing() * 8),
                                      ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeY))
                {
                    for (const auto& msg : g_Work.PrevMessages)
                    {
                        ImGui::Bullet();
                        ImGui::TextWrapped(msg.c_str());
                    }
                }
                ImGui::PopStyleColor();
                ImGui::EndChild();
            }

            ImGui::End();
        });
    }
}
