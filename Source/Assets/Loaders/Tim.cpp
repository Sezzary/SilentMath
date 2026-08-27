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

    /** @brief Gets the intended aspect ratio of a TIM asset.
     *
     * @note Some TIMs are squashed and must be stretched horizontally.
     *
     * @param name Name.
     * @param res True resolution.
     * @return Intended aspect ratio.
     */
    static float GetAspectRatio(const std::string& name, const Vector2i& res)
    {
        static const auto TIM_RES_MAP = std::unordered_map<std::string, Vector2i>
        {
            { "TIM/TITLE.TIM",   Vector2i(640, 480) },
            { "TIM/TITLE_E.TIM", Vector2i(640, 480) },
        };

        float aspect = 1.0f;

        // Commpute intended aspect ratio.
        const auto* obtuseTimRes = Find(TIM_RES_MAP, name);
        if (obtuseTimRes != nullptr)
        {
            aspect = (float)(*obtuseTimRes).x / (float)(*obtuseTimRes).y;
        }
        else
        {
            aspect = (float)res.x / (float)res.y;
        }

        return aspect;
    }

    /** @brief Sets an RGBA pixel in an image pixel buffer to a given CLUT color.
     *
     * @param pixels Output image RGBA pixel buffer.
     * @param res Image resolution.
     * @param x X pixel position.
     * @param y Y pixel position.
     * @param color CLUT color to process and set..
     * @param palette `true` if the image is a color palette atlas, `false` otherwise.
     */
    static void SetPixelColor(std::vector<byte>& pixels, const Vector2i& res, int x, int y, uint16 color,
                              bool isPaletteAtlas = false)
    {
        constexpr int TRANSPARENT_COLOR_FLAG = 1 << 15;

        // Set extracted RGBA pixel components.
        auto pixel = ToSpan(&pixels[((y * res.x) + x) * RGBA_COMP_COUNT], RGBA_COMP_COUNT);
        pixel[0]   = (color & 0x1F) << 3;         // R.
        pixel[1]   = ((color >> 5) & 0x1F) << 3;  // G.
        pixel[2]   = ((color >> 10) & 0x1F) << 3; // B.

        // Handle color palette atlas image.
        if (isPaletteAtlas)
        {
            pixel[3] = 255; // A.
        }
        // Handle regular texture image.
        else
        {
            // Interpret R0, G248, B0 as black. @todo Check if this is really required for some textures.
            if (pixel[0] == 0   && // R.
                pixel[1] == 248 && // G.
                pixel[2] == 0)     // B.
            {
                pixel[1] = 0; // G.
            }

            pixel[3] = (color & TRANSPARENT_COLOR_FLAG) ? 0 : 255; // A.
        }
    };

    /** @brief Sets an indexed pixel in an image pixel buffer to a given palette color index.
     *
     * @note The index is stored in the red color channel.
     *
     * @param pixels Output image indexed pixel buffer.
     * @param res Image resolution.
     * @param x X pixel position.
     * @param y Y pixel position.
     * @param idx Palette color index.
     */
    static void SetPixelIndex(std::vector<byte>& pixels, const Vector2i& res, int x, int y, int idx)
    {
        // Set pixel color palette index.
        auto pixel = ToSpan(&pixels[((y * res.x) + x) * RGBA_COMP_COUNT], RGBA_COMP_COUNT);
        pixel[0]   = idx; // Color index.
        pixel[1]   = 0;   // Unused.
        pixel[2]   = 0;   // Unused.
        pixel[3]   = 255; // A.
    }

    std::shared_ptr<void> ParseTim(const stdfs::path& filename)
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

        // Read CLUT and create palette atlas image.
        auto paletteAtlas = std::optional<TimPaletteAtlas>();
        if (flags & (int)TimFlags::HasClut)
        {
            // Read size.
            uint32 size = stream.ReadUint32();

            // Read frame buffer coordinates (unused).
            uint16 coordX = stream.ReadUint16();
            uint16 coordY = stream.ReadUint16();

            // Read dimensions.
            uint16 width  = stream.ReadUint16();
            uint16 height = stream.ReadUint16();

            // Read color values.
            uint colorCount = width * height;
            auto clut       = std::vector<uint16>(colorCount);
            stream.ReadArray(ToSpan(clut));

            // Create palette atlas.
            paletteAtlas = TimPaletteAtlas
            {
                .Resolution = Vector2i(width, height),
                .Pixels     = std::vector<byte>((width * height) * RGBA_COMP_COUNT)
            };

            // Set palette atlas pixels.
            for (int x = 0; x < paletteAtlas->Resolution.x; x++)
            {
                for (int y = 0; y < paletteAtlas->Resolution.y; y++)
                {
                    int    idx   = (y * paletteAtlas->Resolution.x) + x;
                    uint16 color = clut[idx];
                    SetPixelColor(paletteAtlas->Pixels, paletteAtlas->Resolution, x, y, color, true);
                }
            }

            // @debug 
            //stbi_write_png((fs.GetAppDirectory() / Fmt("{}_P.png", filename.filename().string())).string().c_str(),
            //               paletteAtlas->Resolution.x, paletteAtlas->Resolution.y, 4, paletteAtlas->Pixels.data(), paletteAtlas->Resolution.x * 4);
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
                        for (int i = 0; i < 4 && x < res.x; i++, x++)
                        {
                            uint idx = (colors >> (i * 4)) & 0xF;
                            if (paletteAtlas.has_value())
                            {
                                SetPixelIndex(pixels, res, x, y, idx);
                            }
                            else
                            {
                                uint16 color = idx * (0xFFFF / 0xF);
                                SetPixelColor(pixels, res, x, y, color);
                            }
                        }
                        break;
                    }
                    case TimBppType::Bpp8:
                    {
                        // Read color index.
                        uint8 idx = stream.ReadUint8();

                        // Set pixel.
                        if (paletteAtlas.has_value())
                        {
                            SetPixelIndex(pixels, res, x, y, idx);
                        }
                        else
                        {
                            // Grayscale color `[0, 255]`.
                            uint16 color = idx * (0xFFFF / 0xFF);
                            SetPixelColor(pixels, res, x, y, color);
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
            .Resolution   = res,
            .Pixels       = std::move(pixels),
            .PaletteAtlas = std::move(paletteAtlas),
            .AspectRatio  = GetAspectRatio((filename.parent_path().filename() / filename.filename()).string(), res)
        };
        return std::make_shared<TimAsset>(std::move(asset));
    }

    void QueueTimGpuUpload(const Asset& asset)
    {
        auto& renderer = g_App.GetRenderer();

        renderer.QueueTextureUpload(asset.Name);
    }

    void QueueTimGpuRelease(const Asset& asset)
    {
        auto& renderer = g_App.GetRenderer();

        renderer.QueueTextureRelease(asset.Name);
    }
}
