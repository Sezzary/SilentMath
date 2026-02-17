#include "Framework.h"
#include "Assets/Parsers/Ipd.h"

#include "Application.h"
#include "Utils/Stream.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Assets
{
    std::shared_ptr<void> ParseIpd(const std::filesystem::path& filename)
    {
        constexpr uint8 HEADER_MAGIC = 0x14;

        const auto& fs = g_App.GetFilesystem();

        auto asset = IpdAsset{};

        // Read file.
        auto stream = Stream(filename, true, false);
        if (!stream.IsOpen())
        {
            throw std::runtime_error(Fmt("Failed to open IPD `{}`.",
                                         std::filesystem::relative(filename, fs.GetAssetsDirectory()).string()));
        }

        // Read header magic.
        uint8 magic = stream.ReadUint8();
        if (magic != HEADER_MAGIC)
        {
            throw std::runtime_error(Fmt("Failed to parse invalid IPD `{}`.",
                                         std::filesystem::relative(filename, fs.GetAssetsDirectory()).string()));
        }

        // Read header.
        uint8  isInitialized       = stream.ReadUint8();
        int8   levelGridX          = stream.ReadInt8();
        int8   levelGridZ          = stream.ReadInt8();
        uint32 lmHeaderOffset      = stream.ReadUint32();
        uint8  modelCount          = stream.ReadUint8();
        uint8  modelBufferCount    = stream.ReadUint8();
        uint8  modelOrderCount     = stream.ReadUint8();
        stream.Skip(9); // Unknown.
        uint32 modelInfoOffset     = stream.ReadUint32();
        uint32 modelBuffersOffset  = stream.ReadUint32();
        uint8  texsOffset          = stream.ReadUint8();
        stream.Skip(51); // Unknown.
        uint32 modelOrderPtrOffset = stream.ReadUint32();

        // @todo

        return std::make_shared<IpdAsset>(std::move(asset));
    }
}
