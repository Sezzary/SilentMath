#include "Framework.h"
#include "Assets/Loaders/Dat.h"

#include "Application.h"
#include "Services/Filesystem.h"
#include "Utils/Stream.h"

using namespace Silent::Services;
using namespace Silent::Utils;

namespace Silent::Assets
{
    struct DatFrame
    {

    };

    std::shared_ptr<void> ParseDat(const stdfs::path& filename)
    {
        const auto& fs = g_App.GetFilesystem();

        auto stateFilename = filename;
        auto playFilename  = filename;

        auto name = playFilename.filename().string();
        if (name.starts_with("DEMO"))
        {
            name.replace(0, 4, "PLAY");
            playFilename.replace_filename(name);
        }
        else
        {
            throw std::runtime_error(Fmt("Failed to open DAT `{}`. The file's name must start with `DEMO`,\
                                          which contains the header.\
                                          The associated `PLAY` file, containing playback frames,\
                                          is read in tandem automatically.",
                                         stdfs::relative(filename, fs.GetAssetsDirectory()).string()));
        }

        // Read state file.
        auto stateStream = Stream(stateFilename, true, false);
        if (!stateStream.IsOpen())
        {
            throw std::runtime_error(Fmt("Failed to open DAT `{}`.",
                                         stdfs::relative(stateFilename, fs.GetAssetsDirectory()).string()));
        }

        // Read playback file.
        auto playStream = Stream(playFilename, true, false);
        if (!playStream.IsOpen())
        {
            throw std::runtime_error(Fmt("Failed to open DAT `{}`.",
                                         stdfs::relative(playFilename, fs.GetAssetsDirectory()).string()));
        }

        // @todo Read options config.
        stateStream.Skip(56);

        stateStream.Skip(200);

        // @todo Read savegame.
        stateStream.Skip(636);

        stateStream.Skip(1148);

        uint32 frameCount = stateStream.ReadUint32();
        uint16 randSeed   = stateStream.ReadUint16();

        // Read frames.
        auto frames = std::vector<DatFrame>{};
        frames.reserve(frameCount);
        for (int i = 0; i < frameCount; i++)
        {
            // @todo Read controller state.
            playStream.Skip(15);

            int8 expectedGameState = playStream.ReadInt8();
            int8 videoPresentInterval = playStream.ReadInt8();

            playStream.Skip(2);

            uint32 randSeed = playStream.ReadUint32();
        }

        // @todo Convert everything to a new standard somehow.

        return std::make_shared<DatAsset>(DatAsset
        {

        });
    }
}
