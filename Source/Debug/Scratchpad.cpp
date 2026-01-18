#include "Framework.h"
#include "Debug/Scratchpad.h"

#include "Application.h"
#include "Debug/Debug.h"
#include "Input/Input.h"
#include "Renderer/Common/Enums.h"
#include "Renderer/Renderer.h"
#include "Services/Options.h"
#include "Utils/Parallel.h"

using namespace Silent::Input;
using namespace Silent::Services;

// `Scratchpad` includes.
#include "Gui/Button.h"

using namespace Silent::Gui;

namespace Silent::Debug
{
    void Scratchpad()
    {
        if constexpr (IS_DEBUG_BUILD)
        {
            auto& input    = g_App.GetInput();
            auto& renderer = g_App.GetRenderer();
            auto& fonts = g_App.GetFonts();

            bool isInit = true;
            if (isInit)
            {
                input.InsertText("Test");
                isInit = false;
            }

            input.UpdateText("Test");
            Message(input.GetText("Test").c_str());

            // ====================================

            // Sprite test.

            // Cursor.
            auto cursorSprite = Sprite2d::CreateSprite2d("TIM/HERO_PIC.TIM", Vector2::Zero, Vector2::One,
                                                         input.GetCursorPosition(), 0.0f, Vector2(0.1f, 0.1f), Color::Clear, 0,
                                                         AlignMode::TopLeft, ScaleMode::LongEdge, BlendMode::Subtract);
            renderer.SubmitSprite2d(cursorSprite);

            auto sprite0 = Sprite2d::CreateSprite2d("1ST/2ZANKO_E.TIM", Vector2::Zero, Vector2::One,
                                                    Vector2(25.0f, 50.0f), 0.0f, Vector2(0.5f, 0.25f), Color::Clear, 2,
                                                    AlignMode::Center, ScaleMode::LongEdge, BlendMode::Add);
            renderer.SubmitSprite2d(sprite0);

            auto* font = fonts.GetFont("RetroSerif");
            if (false)
            //if (font != nullptr)
            {
                float msgScale     = 1.0f / 14.0f;
                auto  shadowOffset = SCREEN_SPACE_RES * (1.0f / RETRO_SCREEN_SPACE_RES.y);

                auto pos = Vector2(10.0f, 10.0f);
                auto color = Color(0.0f, 1.0f, 0.0f, 1.0f);

                auto scaleFactor = SCREEN_SPACE_RES / font->GetPointSize();

                auto glyphOffset =  Vector2::Zero;
                auto shapedText  = font->GetShapedText("Have you seen a little girl?");
                for (const auto& glyph : shapedText.Glyphs)
                {
                    // Compute UVs.
                    auto uvMin = glyph.Metadata.AtlasPosition.ToVector2() / Vector2(Font::ATLAS_SIZE); 
                    auto uvMax = uvMin + (glyph.Metadata.AtlasSize.ToVector2() / Vector2(Font::ATLAS_SIZE));

                    // Compute glyph position.
                    auto relPixelPos = glyphOffset + Vector2(glyph.Metadata.Bearing.x, glyph.Metadata.AtlasSize.y - glyph.Metadata.Bearing.y);
                    auto relGlyphPos = (relPixelPos * scaleFactor) * msgScale;

                    // Compute glyph scale.
                    auto glyphScale = Vector2((float)glyph.Metadata.AtlasSize.x / (float)glyph.Metadata.AtlasSize.y, 1.0f) *
                                      Vector2((float)glyph.Metadata.AtlasSize.y / (float)font->GetPointSize());

                    // Submit glyph sprite.
                    auto glyphSprite  = Sprite2d::CreateSprite2d("RetroSerif0", uvMin, uvMax,
                                                                 pos + relGlyphPos, 0.0f, glyphScale * msgScale, color, 1,
                                                                 AlignMode::BottomLeft, ScaleMode::LongEdge, BlendMode::FastAlpha);
                    auto shadowSprite = Sprite2d::CreateSprite2d("RetroSerif0", uvMin, uvMax,
                                                                 (pos + shadowOffset) + relGlyphPos, 0.0f, glyphScale * msgScale, color, 1,
                                                                 AlignMode::BottomLeft, ScaleMode::LongEdge, BlendMode::FastAlpha);
                    renderer.SubmitSprite2d(glyphSprite);
                    //renderer.SubmitSprite2d(shadowSprite);

                    // Update horizontal offset.
                    glyphOffset.x += glyph.Kerning;
                }
            }

            return;

            // GUI button test.
            static auto but = Button(Vector2(25.0f, 25.0f), Vector2(25.0f, 25.0f), ScaleMode::ShortEdge,
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
                                     
                                        auto quad = Shape2d::CreateQuad(Vector2(0.0f,  00.0f),
                                                                            Vector2(0.0f,  50.0f),
                                                                            Vector2(50.0f, 50.0f) * aspectCorrection,
                                                                            Vector2(50.0f, 0.0f) * aspectCorrection,
                                                                            Color(0.0f, 1.0f, 0.2f, 0.4f),
                                                                            Color(0.0f, 1.0f, 0.2f, 0.4f),
                                                                            Color(0.0f, 1.0f, 0.2f, 0.4f),
                                                                            Color(0.0f, 1.0f, 0.2f, 0.4f),
                                                                            0, ScaleMode::ShortEdge, BlendMode::Alpha);
                                        renderer.SubmitShape2d(quad);
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
                                     
                                        auto quad = Shape2d::CreateQuad(Vector2(0.0f,  00.0f),
                                                                            Vector2(0.0f,  50.0f),
                                                                            Vector2(50.0f, 50.0f) * aspectCorrection,
                                                                            Vector2(50.0f, 0.0f) * aspectCorrection,
                                                                            Color(1.0f, 0.0f, 0.4f, 0.4f),
                                                                            Color(1.0f, 0.0f, 0.4f, 0.4f),
                                                                            Color(1.0f, 0.0f, 0.4f, 0.4f),
                                                                            Color(1.0f, 0.0f, 0.4f, 0.4f),
                                                                            0, ScaleMode::ShortEdge, BlendMode::Alpha);
                                        renderer.SubmitShape2d(quad);
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
            auto tri0 = Shape2d::CreateTriangle(Vector2(0.0f + 0.2f, 0.5f + 0.2f),
                                                    Vector2(-0.5f + 0.2f, -0.5f + 0.2f),
                                                    Vector2(0.5f + 0.2f, -0.5f + 0.2f),
                                                    Color(1.0f, 0.0f, 1.0f, 0.5f),
                                                    Color(1.0f, 1.0f, 1.0f, 0.5f),
                                                    Color(1.0f, 0.0f, 1.0f, 0.5f),
                                                    0);
            auto tri1 = Shape2d::CreateTriangle(Vector2(0.2f, 0.25f),
                                                    Vector2(-0.25f, -0.25f),
                                                    Vector2(0.25f, -0.25f),
                                                    Color(1.0f, 0.0f, 0.0f, 0.75f),
                                                    Color(0.0f, 1.0f, 1.0f, 0.75f),
                                                    Color(0.0f, 0.0f, 1.0f, 0.75f),
                                                    0);
            auto quad = Shape2d::CreateQuad(Vector2(40.0f, 40.0f),
                                                Vector2(50.0f, 40.0f),
                                                Vector2(50.0f, 50.0f),
                                                Vector2(40.0f, 50.0f),
                                                Color(0.0f, 0.0f, 0.0f, 0.0f),
                                                Color(0.0f, 1.0f, 0.0f, 1.0f),
                                                Color(0.0f, 1.0f, 0.0f, 1.0f),
                                                Color(0.0f, 0.0f, 0.0f, 0.0f),
                                                0, ScaleMode::ShortEdge, BlendMode::Alpha);
            auto line0 = Shape2d::CreateLine(Vector2i(10, 10),
                                                Vector2i(50, 10),
                                                Color(1.0f, 1.0f, 0.0f, 1.0f),
                                                Color(0.0f, 0.0f, 0.0f, 0.0f),
                                                0);
            auto line1 = Shape2d::CreateLine(Vector2i(15, 11),
                                                Vector2i(70, 11),
                                                Color(1.0f, 1.0f, 0.0f, 1.0f),
                                                Color(1.0f, 1.0f, 0.0f, 1.0f),
                                                0);
            auto line2 = Shape2d::CreateLine(Vector2i(0, 1),
                                                 Vector2i(0, 239),
                                                 Color(1.0f, 1.0f, 0.0f, 1.0f),
                                                 Color(1.0f, 1.0f, 0.0f, 1.0f),
                                                 0);
            auto line3 = Shape2d::CreateLine(Vector2i(319, 0),
                                                 Vector2i(1, 0),
                                                 Color(1.0f, 1.0f, 0.0f, 1.0f),
                                                 Color(1.0f, 1.0f, 0.0f, 1.0f),
                                                 0);
            //renderer.SubmitShape2d(tri0);
            //renderer.SubmitShape2d(tri1);
            renderer.SubmitShape2d(quad);
            renderer.SubmitShape2d(line0);
            renderer.SubmitShape2d(line1);
            //renderer.SubmitShape2d(line2);
            renderer.SubmitShape2d(line3);

            for (int i = 0; i < 11; i++)
            {
                auto line3 = Shape2d::CreateLine(Vector2i(39,  82 + (i * 16)),
                                                     Vector2i(200, 82 + (i * 16)),
                                                     Color::From8Bit(176, 176, 176),
                                                     Color::From8Bit(160, 128, 64),
                                                     0);
                auto quadB0 = Shape2d::CreateQuad(
                                                    Vector2i(52, 69 + (i * 16)),
                                                    Vector2i(52, 81 + (i * 16)),
                                                    Vector2i(40, 69 + (i * 16)),
                                                    Vector2i(40, 81 + (i * 16)),
                                                    Color::From8Bit(255, 255, 255),
                                                    Color::From8Bit(160, 128, 64),
                                                    Color::From8Bit(160, 128, 64),
                                                    Color::From8Bit(255, 255, 255),
                                                    0);
                auto quadB1 = Shape2d::CreateQuad(
                                                    Vector2i(39, 68 + (i * 16)),
                                                    Vector2i(39, 82 + (i * 16)),
                                                    Vector2i(53, 68 + (i * 16)),
                                                    Vector2i(53, 82 + (i * 16)),
                                                    Color::From8Bit(255, 255, 255),
                                                    Color::From8Bit(160, 128, 64),
                                                    Color::From8Bit(160, 128, 64),
                                                    Color::From8Bit(64,  64,  64),
                                                    1);
                renderer.SubmitShape2d(line3);
                renderer.SubmitShape2d(quadB1);
                renderer.SubmitShape2d(quadB0);
            }

            auto tri = Shape2d::CreateTriangle(
                                                   Vector2i(8, 84),
                                                   Vector2i(16, 76),
                                                   Vector2i(16, 92),
                                                   Color::From8Bit(48, 255, 255),
                                                   Color::From8Bit(48, 255, 128),
                                                   Color::From8Bit(48, 255, 128),
                                                   0);
            renderer.SubmitShape2d(tri);
        }
    }
}
