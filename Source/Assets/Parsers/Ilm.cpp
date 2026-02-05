#include "Framework.h"
#include "Assets/Parsers/Ilm.h"

#include "Utils/Stream.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Assets
{
    std::shared_ptr<void> ParseIlm(const std::filesystem::path& filename)
    {
        constexpr int HEADER_MAGIC       = 0x3006;
        constexpr int HEADER_NAME_OFFSET = 0x14;

        // Read file.
        auto stream = Stream(filename, true, false);
        if (!stream.IsOpen())
        {
            throw std::runtime_error(Fmt("Failed to open ILM `{}`.", filename.string()));
        }

        // Read header magic.
        int16 magic = stream.ReadInt16();
        if (magic != HEADER_MAGIC)
        {
            throw std::runtime_error(Fmt("Failed to parse invalid ILM `{}`.", filename.string()));
        }

        uint8 isInitialized = stream.ReadUint8();
        stream.Skip(1);

        // Read name offset.
        uint32 nameOffset = stream.ReadUint32();
        if (nameOffset != HEADER_NAME_OFFSET)
        {
            throw std::runtime_error(Fmt("Attempted to parse ILM `{}` with incongruent name offset.", filename.string()));
        }

        // Read attributes.
        uint32 objectCount   = stream.ReadUint32();
        uint32 objectsOffset = stream.ReadUint32();
        uint32 idTableOffset = stream.ReadUint32();
        auto   name          = stream.ReadNullString();

        // Read objects.
        stream.SetPosition(objectsOffset);
        for (int i = 0; i < objectCount; i++)
        {

        }

        // Read ID table.
        stream.SetPosition(idTableOffset);
        for (int i = 0; i < objectCount; i++)
        {

        }

        auto asset = IlmAsset{};

        return std::make_shared<IlmAsset>(std::move(asset));
    }
}
