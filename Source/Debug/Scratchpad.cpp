#include "Framework.h"
#include "Debug/Scratchpad.h"

#include "Application.h"
#include "Debug/Debug.h"
#include "Input/Input.h"
#include "Renderer/Common/Enums.h"
#include "Renderer/Renderer.h"
#include "Services/Options.h"
#include "Utils/Parallel.h"

using namespace Silent::Services;

// `Scratchpad` includes.
#ifdef _DEBUG
    #include "Gui/Button.h"

    using namespace Silent::Gui;
#endif

namespace Silent::Debug
{
    void Scratchpad()
    {
        if constexpr (IS_DEBUG_BUILD)
        {
            auto& input = g_App.GetInput();

            bool isInit = true;
            if (isInit)
            {
                input.InsertText("Test");
                isInit = false;
            }

            input.UpdateText("Test");
            Message(input.GetText("Test").c_str());

            // ====================================

            auto& renderer = g_App.GetRenderer();

            // Sprite test.
            renderer.SubmitScreenSprite(0, Vector2::Zero, Vector2::Zero, Vector2::Zero, 0, Vector2::Zero, Color::Clear, 0,
                                        AlignMode::Center, ScaleMode::Fill, BlendMode::Opaque);

            // GUI button test.
            static auto but = Button(Vector2(25.0f, 75.0f), Vector2(25.0f, 25.0f), ScaleMode::Fit,
                                     []() { Debug::Log("Entering!"); },
                                     [&]()
                                     {
                                        auto& renderer2 = g_App.GetRenderer();
                                        auto  res    = renderer.GetScreenResolution().ToVector2();
                                        float aspect = res.x / res.y;
                                        auto aspectCorrection = Vector2::One;
                                        if (aspect >= 1.0f)
                                        {
                                            aspectCorrection.x = 1.0f / aspect;
                                        }
                                        else
                                        {
                                            aspectCorrection.y = 1.0f / (1.0f / aspect);
                                        }
                                     
                                        auto quad = Primitive2d::CreateQuad(Vector2(0.0f,  00.0f),
                                                                            Vector2(0.0f,  50.0f),
                                                                            Vector2(50.0f, 50.0f) * aspectCorrection,
                                                                            Vector2(50.0f, 0.0f) * aspectCorrection,
                                                                            Color(0.0f, 1.0f, 0.2f, 0.4f),
                                                                            Color(0.0f, 1.0f, 0.2f, 0.4f),
                                                                            Color(0.0f, 1.0f, 0.2f, 0.4f),
                                                                            Color(0.0f, 1.0f, 0.2f, 0.4f),
                                                                            0, ScaleMode::Fit, BlendMode::Alpha);
                                        renderer.Submit2dPrimitive(quad);
                                     },
                                     []() { Debug::Log("Leaving!"); },
                                     [&]()
                                     {
                                        auto& renderer2 = g_App.GetRenderer();
                                        auto  res    = renderer.GetScreenResolution().ToVector2();
                                        float aspect = res.x / res.y;
                                        auto aspectCorrection = Vector2::One;
                                        if (aspect >= 1.0f)
                                        {
                                            aspectCorrection.x = 1.0f / aspect;
                                        }
                                        else
                                        {
                                            aspectCorrection.y = 1.0f / (1.0f / aspect);
                                        }
                                     
                                        auto quad = Primitive2d::CreateQuad(Vector2(0.0f,  00.0f),
                                                                            Vector2(0.0f,  50.0f),
                                                                            Vector2(50.0f, 50.0f) * aspectCorrection,
                                                                            Vector2(50.0f, 0.0f) * aspectCorrection,
                                                                            Color(1.0f, 0.0f, 0.4f, 0.4f),
                                                                            Color(1.0f, 0.0f, 0.4f, 0.4f),
                                                                            Color(1.0f, 0.0f, 0.4f, 0.4f),
                                                                            Color(1.0f, 0.0f, 0.4f, 0.4f),
                                                                            0, ScaleMode::Fit, BlendMode::Alpha);
                                        renderer.Submit2dPrimitive(quad);
                                     },
                                     []() { Debug::Log("Clicking!"); },
                                     []() { Debug::Log("Holding!"); },
                                     []() { Debug::Log("Releasing!"); });

            // Check if power menu is enabled.
            if (!Debug::g_Work.EnablePowerMenu)
            {
                but.Update(input.GetCursorPosition());
            }

            // @temp
            auto tri0 = Primitive2d::CreateTriangle(Vector2(0.0f + 0.2f, 0.5f + 0.2f),
                                                    Vector2(-0.5f + 0.2f, -0.5f + 0.2f),
                                                    Vector2(0.5f + 0.2f, -0.5f + 0.2f),
                                                    Color(1.0f, 0.0f, 1.0f, 0.5f),
                                                    Color(1.0f, 1.0f, 1.0f, 0.5f),
                                                    Color(1.0f, 0.0f, 1.0f, 0.5f),
                                                    0);
            auto tri1 = Primitive2d::CreateTriangle(Vector2(0.2f, 0.25f),
                                                    Vector2(-0.25f, -0.25f),
                                                    Vector2(0.25f, -0.25f),
                                                    Color(1.0f, 0.0f, 0.0f, 0.75f),
                                                    Color(0.0f, 1.0f, 1.0f, 0.75f),
                                                    Color(0.0f, 0.0f, 1.0f, 0.75f),
                                                    0);
            auto quad = Primitive2d::CreateQuad(Vector2(40.0f, 40.0f),
                                                Vector2(50.0f, 40.0f),
                                                Vector2(50.0f, 50.0f),
                                                Vector2(40.0f, 50.0f),
                                                Color(0.0f, 0.0f, 0.0f, 0.0f),
                                                Color(0.0f, 1.0f, 0.0f, 1.0f),
                                                Color(0.0f, 1.0f, 0.0f, 1.0f),
                                                Color(0.0f, 0.0f, 0.0f, 0.0f),
                                                0, ScaleMode::Fit, BlendMode::Alpha);
            auto line0 = Primitive2d::CreateLine(Vector2i(10, 10),
                                                Vector2i(50, 10),
                                                Color(1.0f, 1.0f, 0.0f, 1.0f),
                                                Color(0.0f, 0.0f, 0.0f, 0.0f),
                                                0);
            auto line1 = Primitive2d::CreateLine(Vector2i(15, 11),
                                                Vector2i(70, 11),
                                                Color(1.0f, 1.0f, 0.0f, 1.0f),
                                                Color(1.0f, 1.0f, 0.0f, 1.0f),
                                                0);
            auto line2 = Primitive2d::CreateLine(Vector2i(0, 1),
                                                 Vector2i(0, 239),
                                                 Color(1.0f, 1.0f, 0.0f, 1.0f),
                                                 Color(1.0f, 1.0f, 0.0f, 1.0f),
                                                 0);
            auto line3 = Primitive2d::CreateLine(Vector2i(319, 0),
                                                 Vector2i(1, 0),
                                                 Color(1.0f, 1.0f, 0.0f, 1.0f),
                                                 Color(1.0f, 1.0f, 0.0f, 1.0f),
                                                 0);
            //renderer.Submit2dPrimitive(tri0);
            //renderer.Submit2dPrimitive(tri1);
            renderer.Submit2dPrimitive(quad);
            renderer.Submit2dPrimitive(line0);
            renderer.Submit2dPrimitive(line1);
            //renderer.Submit2dPrimitive(line2);
            renderer.Submit2dPrimitive(line3);

            for (int i = 0; i < 11; i++)
            {
                auto line3 = Primitive2d::CreateLine(Vector2i(39,  82 + (i * 16)),
                                                     Vector2i(200, 82 + (i * 16)),
                                                     Color::From8Bit(176, 176, 176),
                                                     Color::From8Bit(160, 128, 64),
                                                     0);
                auto quadB0 = Primitive2d::CreateQuad(
                                                    Vector2i(52, 69 + (i * 16)),
                                                    Vector2i(52, 81 + (i * 16)),
                                                    Vector2i(40, 69 + (i * 16)),
                                                    Vector2i(40, 81 + (i * 16)),
                                                    Color::From8Bit(255, 255, 255),
                                                    Color::From8Bit(160, 128, 64),
                                                    Color::From8Bit(160, 128, 64),
                                                    Color::From8Bit(255, 255, 255),
                                                    0);
                auto quadB1 = Primitive2d::CreateQuad(
                                                    Vector2i(39, 68 + (i * 16)),
                                                    Vector2i(39, 82 + (i * 16)),
                                                    Vector2i(53, 68 + (i * 16)),
                                                    Vector2i(53, 82 + (i * 16)),
                                                    Color::From8Bit(255, 255, 255),
                                                    Color::From8Bit(160, 128, 64),
                                                    Color::From8Bit(160, 128, 64),
                                                    Color::From8Bit(64,  64,  64),
                                                    1);
                renderer.Submit2dPrimitive(line3);
                renderer.Submit2dPrimitive(quadB1);
                renderer.Submit2dPrimitive(quadB0);
            }

            auto tri = Primitive2d::CreateTriangle(
                                                   Vector2i(8, 84),
                                                   Vector2i(16, 76),
                                                   Vector2i(16, 92),
                                                   Color::From8Bit(48, 255, 255),
                                                   Color::From8Bit(48, 255, 128),
                                                   Color::From8Bit(48, 255, 128),
                                                   0);
            renderer.Submit2dPrimitive(tri);
        }
    }
}
