#include "Framework.h"
#include "Assets/Loaders/Tim.h"

#include "Application.h"
#include "Assets/AssetStreamer.h"
#include "Renderer/Common/Constants.h"
#include "Renderer/Renderer.h"
#include "Utils/Stream.h"
#include "Utils/Utils.h"

using namespace Silent::Renderer;
using namespace Silent::Utils;

namespace Silent::Assets
{
    /** @brief TIM bits per pixel types. */
    enum class TimBppType
    {
        Bpp4,
        Bpp8,
        Bpp16
    };

    /** @brief TIM color flags. */
    enum class TimFlags
    {
        Bpp4    = 0,
        Bpp8    = 1 << 0,
        Bpp16   = 1 << 1,
        HasClut = 1 << 3
    };

    /** @brief Sets an RGBA pixel in an image pixel buffer to a given CLUT color.
     *
     * @param pixels Output image RGBA pixel buffer.
     * @param res Image resolution.
     * @param x X pixel position.
     * @param y Y pixel position.
     * @param color CLUT color.
     */
    static void SetPixelColor(std::vector<byte>& pixels, const Vector2i& res, int x, int y, uint16 color)
    {
        constexpr int TRANSPARENT_COLOR_FLAG = 1 << 15;

        // Collect extracted RGBA components.
        byte* out = &pixels[((y * res.x) + x) * RGBA_COMP_COUNT];
        out[0]    = (color & 0x1F) << 3;                        // R.
        out[1]    = ((color >> 5) & 0x1F) << 3;                 // G.
        out[2]    = ((color >> 10) & 0x1F) << 3;                // B.
        out[3]    = 255;//(color & TRANSPARENT_COLOR_FLAG) ? 255 : 0; // A.

        // Interpret R0, G248, B0 as black. @todo Check if this is really required for some textures.
        //if (out[0] == 0   && // R.
        //    out[1] == 248 && // G.
        //    out[2] == 0)     // B.
        //{
        //    out[1] = 0; // G.
        //}
    };

    std::shared_ptr<void> TimParse(const stdfs::path& filename)
    {
        constexpr int HEADER_MAGIC = 0x10;
        constexpr int BPP_MASK     = 0x7;

        const auto& fs = g_App.GetFilesystem();

        // Read file.
        auto stream = Stream(filename, true, false);
        if (!stream.IsOpen())
        {
            throw std::runtime_error(Fmt("Failed to open TIM `{}`.",
                                         stdfs::relative(filename, fs.GetAssetsDirectory()).string()));
        }

        // Confirm TIM format magic.
        uint32 magic = stream.ReadUint32();
        if (magic != HEADER_MAGIC)
        {
            throw std::runtime_error(Fmt("Failed to parse invalid TIM `{}`.",
                                         stdfs::relative(filename, fs.GetAssetsDirectory()).string()));
        }

        // Read CLUT and BPP flags.
        uint32 flags = stream.ReadUint32();

        // Read CLUT.
        auto clut    = std::vector<uint16>{};
        auto palette = std::optional<TimPalette>();
        if (flags & (int)TimFlags::HasClut)
        {
            // Read size.
            uint32 size = stream.ReadUint32();

            // Read frame buffer coordinates (unused).
            uint16 coordX = stream.ReadUint16();
            uint16 coordY = stream.ReadUint16();

            // Read dimensions.
            uint16 width = stream.ReadUint16();
            uint16 height = stream.ReadUint16();

            // Read color values.
            uint colorCount = width * height;
            clut.resize(colorCount);
            stream.ReadArray(ToSpan(clut));

            // Create palette.
            palette = TimPalette
            {
                .Resolution = Vector2i(width, height),
                .Pixels     = std::vector<byte>((width * height) * RGBA_COMP_COUNT)
            };

            // Set palette pixels.
            for (int x = 0; x < palette->Resolution.x; x++)
            {
                for (int y = 0; y < palette->Resolution.y; y++)
                {
                    int    colorIdx = (y * palette->Resolution.x) + x;
                    uint16 color    = clut[colorIdx];
                    SetPixelColor(palette->Pixels, palette->Resolution, x, y, color);
                }
            }

            // @debug Write 
            //stbi_write_png((g_App.GetFilesystem().GetAppDirectory() / Fmt("{}_P.png", filename.filename().string())).string().c_str(),
            //                palette->Resolution.x, palette->Resolution.y, 4, palette->Pixels.data(), palette->Resolution.x * 4);
        }

        // Read image size (unused).
        uint32 size = stream.ReadUint32();

        // Read image frame buffer coordinates (unused).
        uint16 coordX = stream.ReadUint16();
        uint16 coordY = stream.ReadUint16();

        // Read image dimensions.
        uint16 width  = stream.ReadUint16();
        uint16 height = stream.ReadUint16();

        // Define BPP.
        auto bpp = TimBppType::Bpp4;
        switch ((TimFlags)(flags & BPP_MASK))
        {
            case TimFlags::Bpp4:
            {
                bpp = TimBppType::Bpp4;
                break;
            }
            case TimFlags::Bpp8:
            {
                bpp = TimBppType::Bpp8;
                break;
            }
            case TimFlags::Bpp16:
            {
                bpp = TimBppType::Bpp16;
                break;
            }
            default:
            {
                std::runtime_error("Failed to read TIM with no BPP flags.");
            }
        }

        // Define image width coefficient based on BPP.
        int widthCoeff = 1;
        switch (bpp)
        {
            case TimBppType::Bpp4:
            {
                widthCoeff = 4;
                break;
            }
            case TimBppType::Bpp8:
            {
                widthCoeff = 2;
                break;
            }
            case TimBppType::Bpp16:
            {
                widthCoeff = 1;
                break;
            }
        }

        // Define image resolution.
        auto res = Vector2i(width * widthCoeff, height);

        // Prepare image pixels.
        auto pixels = std::vector<byte>((res.x * res.y) * RGBA_COMP_COUNT);

        // Read image pixels.
        for (int y = 0; y < res.y; y++)
        {
            for (int x = 0; x < res.x;)
            {
                switch (bpp)
                {
                    default:
                    case TimBppType::Bpp4:
                    {
                        // Read colors.
                        uint16 colors = stream.ReadUint16();

                        // Set pixel.
                        for (int j = 0; j < 4 && x < res.x; j++, x++)
                        {
                            uint idx = (colors >> (j * 4)) & 0xF;
                            if (!palette.has_value())
                            {
                                uint16 color = idx * (0xFFFF / 0xF);
                                SetPixelColor(pixels, res, x, y, color);
                            }
                            else
                            {
                                byte* out = &pixels[((y * res.x) + x) * RGBA_COMP_COUNT];
                                out[0]    = idx; // Color index.
                                out[1]    = 0;   // Unused.
                                out[2]    = 0;   // Unused.
                                out[3]    = 255; // A.
                            }
                        }
                        break;
                    }
                    case TimBppType::Bpp8:
                    {
                        // Read color index.
                        uint8 idx = stream.ReadUint8();
                        
                        // Set pixel.
                        if (!palette.has_value())
                        {
                            // Grayscale color `[0, 255]`.
                            uint16 color = idx * (0xFFFF / 0xFF);
                            SetPixelColor(pixels, res, x, y, color);
                        }
                        else
                        {
                            byte* out = &pixels[((y * res.x) + x) * RGBA_COMP_COUNT];
                            out[0]    = idx; // Color index.
                            out[1]    = 0;   // Unused.
                            out[2]    = 0;   // Unused.
                            out[3]    = 255; // A.
                        }

                        x++;
                        break;
                    }
                    case TimBppType::Bpp16:
                    {
                        // Read color.
                        uint16 color = stream.ReadUint16();

                        // Set pixel.
                        SetPixelColor(pixels, res, x, y, color);

                        x++;
                        break;
                    }
                }
            }
        }

        // Create asset.
        auto asset = TimAsset
        {
            .Resolution  = res,
            .Pixels      = std::move(pixels),
            .Palette     = std::move(palette),
            .AspectRatio = (float)res.x / (float)res.y
        };
        return std::make_shared<TimAsset>(std::move(asset));
    }

    void TimQueueGpuUpload(const Asset& asset)
    {
        auto& renderer = g_App.GetRenderer();

        renderer.QueueTextureUpload(asset.Name);
    }

    void TimQueueGpuRelease(const Asset& asset)
    {
        auto& renderer = g_App.GetRenderer();

        renderer.QueueTextureRelease(asset.Name);
    }
}
