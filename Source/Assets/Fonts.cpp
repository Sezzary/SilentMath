#include "Framework.h"
#include "Assets/Fonts.h"

#include "Utils/Font.h"

using namespace Silent::Utils;

namespace Silent::Assets
{
    // @todo Could be defined in a Lua script instead.
    const std::vector<FontMetadata> FONTS_METADATA
    {
        {
            .Name      = "SmoothSerif",
            .Filenames =
            {
                "FreeSerif.ttf",
                "NotoSerifJP-Medium.ttf",
                "NotoSerifKR-Medium.ttf"
            },
            .PointSize = 64,
            .Trackings =
            {
                0.1f,
                0.0f,
                0.0f
            },
            .EnableAntialiasing = true
        },
        {
            .Name      = "RetroSerif",
            .Filenames =
            {
                "Times_16.ttf",
                "PsxBios.ttf",
                "UnifontExMono.ttf"
            },
            .PointSize = 16,
            .Trackings =
            {
                0.0f,
                0.0f,
                0.0f
            },
            .EnableAntialiasing = false
        },
        {
            .Name      = "SmoothSans",
            .Filenames =
            {
                "NotoSans-Bold.ttf",
                "NotoSansJP-Bold.ttf",
                "NotoSansKR-Bold.ttf"
            },
            .PointSize = 40,
            .Trackings =
            {
                0.0f,
                0.0f,
                0.0f
            },
            .EnableAntialiasing = true
        },
        {
            .Name      = "RetroSans",
            .Filenames =
            {
                "SilentControl10.ttf",
                "PixelMplus10-Regular.ttf",
                "dalmoori10.ttf"
            },
            .PointSize = 10,
            .Trackings =
            {
                0.0f,
                0.0f,
                0.0f
            },
            .EnableAntialiasing = false
        },
        {
            .Name      = "Handwriting",
            .Filenames = 
            {
                "FreeSansItalic.ttf"
                // @todo Need others for Japanese and Korean.
            },
            .PointSize = 64,
            .Trackings =
            {
                0.0f,
                0.0f,
                0.0f
            },
            .EnableAntialiasing = true
        }
    };
}
