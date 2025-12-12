#include "Framework.h"
#include "Assets/Fonts.h"

#include "Utils/Font.h"

using namespace Silent::Utils;

namespace Silent::Assets
{
    /** @brief String color IDs for strings displayed in screen space.
     * Used as indices into `STRING_COLORS`.
     */
    enum class e_StringColorId
    {
        Gold,
        DarkGrey,
        Green,
        Nuclear,
        Red,
        LightGrey,
        White,

        Count
    };

    constexpr std::array<Color, (int)e_StringColorId::Count> STRING_COLORS =
    {
        Color::From8Bit(160, 128, 64),
        Color::From8Bit(32,  32,  32),
        Color::From8Bit(24,  128, 40),
        Color::From8Bit(8,   184, 96),
        Color::From8Bit(128, 0,   0),
        Color::From8Bit(100, 100, 100),
        Color::From8Bit(128, 128, 128)
    };

    // @todo Could be defined in a Lua script instead.
    const std::vector<FontMetadata> FONTS_METADATA
    {
        FontMetadata
        {
            .Name      = "SmoothSerif",
            .Filenames =
            {
                "FreeSerif.otf",
                "NotoSerifJP-Medium.ttf",
                "NotoSerifKR-Medium.ttf"
            },
            .PointSize          = 48,
            .EnableAntialiasing = true
        },
        FontMetadata
        {
            .Name      = "RetroSerif",
            .Filenames =
            {
                "Times_16.ttf",
                "PsxBios.ttf",
                "UnifontExMono.ttf"
            },
            .PointSize          = 16,
            .EnableAntialiasing = false
        },
        FontMetadata
        {
            .Name      = "SmoothSans",
            .Filenames =
            {
                "NotoSans-Bold.ttf",
                "NotoSansJP-Bold.ttf",
                "NotoSansKR-Bold.ttf"
            },
            .PointSize          = 32,
            .EnableAntialiasing = true
        },
        FontMetadata
        {
            .Name      = "RetroSans",
            .Filenames =
            {
                "SilentControl10.ttf",
                "PixelMplus10-Regular.ttf",
                "dalmoori10.ttf"
            },
            .PointSize          = 10,
            .EnableAntialiasing = false
        }
    };
}
