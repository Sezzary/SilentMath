#include "Framework.h"
#include "Assets/Parsers/Tim.h"

#include "Application.h"
#include "Renderer/Common/Constants.h"
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

    std::shared_ptr<void> ParseTim(const std::filesystem::path& filename)
    {
        constexpr int HEADER_MAGIC           = 0x10;
        constexpr int BPP_MASK               = 0x7;
        constexpr int TRANSPARENT_COLOR_FLAG = 1 << 15;

        const auto& fs = g_App.GetFilesystem();

        // Read file.
        auto stream = Stream(filename, true, false);
        if (!stream.IsOpen())
        {
            throw std::runtime_error(Fmt("Failed to open TIM `{}`.",
                                         std::filesystem::relative(fs.GetAssetsDirectory(), filename).string()));
        }

        // Confirm TIM format magic.
        uint32 magic = stream.ReadUint32();
        if (magic != HEADER_MAGIC)
        {
            throw std::runtime_error(Fmt("Failed to parse invalid TIM `{}`.",
                                         std::filesystem::relative(fs.GetAssetsDirectory(), filename).string()));
        }

        // Read CLUT and BPP flags.
        uint32 flags = stream.ReadUint32();

        // Read CLUT.
        auto clut = std::vector<uint16>{};
        if (flags & (int)TimFlags::HasClut)
        {
            // Read size.
            uint32 clutSize = stream.ReadUint32();

            // Read frame buffer coordinates (unused).
            uint16 clutX = stream.ReadUint16();
            uint16 clutY = stream.ReadUint16();

            // Read dimensions.
            uint16 clutW = stream.ReadUint16();
            uint16 clutH = stream.ReadUint16();

            // Read color values.
            uint clutCount = clutW * clutH;
            clut.resize(clutCount);
            stream.ReadArray(ToSpan(clut));
        }

        // Read image data header (unused).
        uint32 imageSize = stream.ReadUint32();;

        // Read frame buffer coordinates (unused).
        uint16 imageX = stream.ReadUint16();
        uint16 imageY = stream.ReadUint16();

        // Read image dimensions.
        uint16 imageW = stream.ReadUint16();
        uint16 imageH = stream.ReadUint16();

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
        auto res = Vector2i(imageW * widthCoeff, imageH);

        // Create asset.
        auto asset = TimAsset
        {
            .Resolution = res,
            .Pixels     = std::vector<byte>((res.x * res.y) * RGBA_COMP_COUNT)
        };

        auto SetPixelColor = [&](int x, int y, uint16 color)
        {
            // Collect extracted RGBA components.
            byte* out = &asset.Pixels[((y * res.x) + x) * RGBA_COMP_COUNT];
            out[0]    = (color & 0x1F) << 3;                        // B.
            out[1]    = ((color >> 5) & 0x1F) << 3;                 // G.
            out[2]    = ((color >> 10) & 0x1F) << 3;                // R.
            out[3]    = (color & TRANSPARENT_COLOR_FLAG) ? 255 : 0; // A.

            // Interpret R0, G248, B0 as black. @todo Check if this is really required for some textures.
            if (out[0] == 0   && // B.
                out[1] == 248 && // G.
                out[2] == 0)     // R.
            {
                out[1] = 0;
            }
        };

        // Read pixels.
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

                        for (int i = 0; i < 4 && x < res.x; i++, x++)
                        {
                            uint idx = (colors >> (i * 4)) & 0xF;
                            if (clut.empty())
                            {
                                uint16 color = idx * (0xFFFF / 0xF);
                                SetPixelColor(x, y, color);
                            }
                            else
                            {
                                uint16 color = clut[idx];
                                SetPixelColor(x, y, color);
                            }
                        }
                        break;
                    }
                    case TimBppType::Bpp8:
                    {
                        // Read color index.
                        uint8 idx = stream.ReadUint8();
                        
                        // Set pixel.
                        if (clut.empty())
                        {
                            // Grayscale color `[0, 255]`.
                            uint16 color = idx * (0xFFFF / 0xFF);
                            SetPixelColor(x, y, color);
                        }
                        else
                        {
                            // CLUT color.
                            uint16 color = clut[idx];
                            SetPixelColor(x, y, color);
                        }

                        x++;
                        break;
                    }
                    case TimBppType::Bpp16:
                    {
                        // Read color.
                        uint16 color = stream.ReadUint16();

                        // Set pixel.
                        SetPixelColor(x, y, color);

                        x++;
                        break;
                    }
                }
            }
        }

        return std::make_shared<TimAsset>(std::move(asset));
    }
}
