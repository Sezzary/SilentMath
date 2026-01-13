#include "Framework.h"
#include "Savegame/Savegame.h"

#include "Application.h"
#include "Assets/TranslationKeys.h"
#include "Savegame/Schemas.h"
#include "Services/Filesystem.h"
#include "Utils/Stream.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Savegame
{
    static const auto SAVE_LOCATION_NAME_KEYS = std::vector<std::string>
    {
        KEY_SAVE_LOC_ANYWHERE,
        KEY_SAVE_LOC_CAFE,
        KEY_SAVE_LOC_BUS,
        KEY_SAVE_LOC_STORE,
        KEY_SAVE_LOC_INFIRMARY,
        KEY_SAVE_LOC_DOGHOUSE,
        KEY_SAVE_LOC_GORDON,
        KEY_SAVE_LOC_CHURCH,
        KEY_SAVE_LOC_GARAGE,
        KEY_SAVE_LOC_POLICE,
        KEY_SAVE_LOC_RECEPTION,
        KEY_SAVE_LOC_ROOM_302,
        KEY_SAVE_LOC_DIRECTORS,
        KEY_SAVE_LOC_JEWELLERY_SHOP,
        KEY_SAVE_LOC_POOL_HALL,
        KEY_SAVE_LOC_ANTIQUE_SHOP,
        KEY_SAVE_LOC_THEME_PARK,
        KEY_SAVE_LOC_BOAT,
        KEY_SAVE_LOC_BRIDGE,
        KEY_SAVE_LOC_MOTEL,
        KEY_SAVE_LOC_LIGHTHOUSE,
        KEY_SAVE_LOC_SEWER,
        KEY_SAVE_LOC_NOWHERE,
        KEY_SAVE_LOC_CHILDS_ROOM,
        KEY_SAVE_LOC_NEXT_FEAR
    };

    const std::vector<SavegameMetadata>& SavegameManager::GetSlotMetadata()
    {
        return _slotMetadata;
    }

    void SavegameManager::Initialize()
    {
        PopulateSlotMetadata();
    }

    void SavegameManager::Save(int fileIdx, int saveIdx)
    {
        // Create savegame buffer.
        auto saveBuffer = ToSavegameBuffer(_savegame);

        // @todo Fill metadata.

        // Ensure directory exists.
        auto saveFile = GetSavegamePath(fileIdx, saveIdx);
        std::filesystem::create_directories(saveFile.parent_path());

        // Write savegame buffer.
        auto stream = Stream(saveFile, false, true);
        if (stream.IsOpen())
        {
            // @todo
            //stream.WriteArray(ToSpan(*saveBuffer));
        }

        Debug::Log(Fmt("Saved game to file {}, savegame {}.", fileIdx + 1, saveIdx + 1), Debug::LogLevel::Info);
    }

    void SavegameManager::Load(int fileIdx, int saveIdx)
    {
        auto saveFile = GetSavegamePath(fileIdx, saveIdx);

        // Open savegame buffer file.
        auto stream = Stream(saveFile, true, false);
        if (!stream.IsOpen())
        {
            Debug::Log(Fmt("Attempted to load missing savegame for file {}, savegame {}.", fileIdx + 1, saveIdx + 1),
                       Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return;
        }

        // Get file size.
        auto fileSize = stream.GetSize();

        // Read file into savegame buffer.
        auto fileBuffer = std::vector<byte>(fileSize);
        stream.ReadArray(ToSpan(fileBuffer));
        auto saveBuffer = struct_pack::deserialize<Schemas::Savegame>(fileBuffer);

        // @todo
        // Read savegame buffer.
        //_savegame = std::move(*FromSavegameBuffer(*saveBuffer));

        Debug::Log(Fmt("Loaded game from file {}, savegame {}.", fileIdx + 1, saveIdx + 1), Debug::LogLevel::Info);
    }

    const Savegame* SavegameManager::operator->() const
    {
        return &_savegame;
    }

    Savegame* SavegameManager::operator->()
    {
        return &_savegame;
    }

    std::filesystem::path SavegameManager::GetSavegamePath(int fileIdx, int saveIdx) const
    {
        const auto& fs = g_App.GetFilesystem();

        auto fileDirName  = Fmt("{}{}", SAVEGAME_FILE_DIR_NAME_BASE, fileIdx + 1);
        auto saveFilename = Fmt("{}{}", saveIdx + 1, SAVEGAME_FILE_EXT);
        return fs.GetSavegameDirectory() / fileDirName / saveFilename;
    }

    SavegameMetadata SavegameManager::GetMetadata(const std::filesystem::path& saveFile) const
    {
        // Open savegame buffer file.
        auto stream = Stream(saveFile, true, false);
        if (!stream.IsOpen())
        {
            Debug::Log(Fmt("Attempted to get metadata for missing savegame file `{}`.", saveFile.string()), Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return SavegameMetadata
            {
                .FileIdx        = NO_VALUE,
                .DataIdx        = NO_VALUE,
                .SaveCount      = NO_VALUE,
                .LocationId     = NO_VALUE,
                .GameplayTimer  = 0,
                .IsNextFearMode = false,
                .Flags          = NO_VALUE
            };
        }

        // Get file size.
        auto fileSize = stream.GetSize();

        // Read file into savegame buffer.
        auto fileBuffer = std::vector<byte>(fileSize);
        stream.Read(fileBuffer.data(), fileSize);
        auto saveBuffer = struct_pack::deserialize<Schemas::Savegame>(fileBuffer);
        
        // @todo Read metadata.
        auto metadata = SavegameMetadata{};

        return metadata;
    }

    void SavegameManager::PopulateSlotMetadata()
    {
        const auto& fs = g_App.GetFilesystem();

        // Collect file folders.
        auto fileDirs = std::vector<std::filesystem::path>{};
        for (const auto& fileDir : std::filesystem::directory_iterator(fs.GetSavegameDirectory()))
        {
            if (fileDir.is_directory())
            {
                fileDirs.push_back(fileDir.path());
            }
        }

        // Sort file folders.
        Sort(fileDirs, [](const std::filesystem::path& file0, const std::filesystem::path& file1)
        {
            auto extractNumber = [](const std::filesystem::path& file)
            {
                auto filename   = file.string(); // "File [ID]".
                auto numericStr = std::string();
                for (char curChar : filename)
                {
                    if (std::isdigit(curChar))
                    {
                        numericStr += curChar;
                    }
                }

                return std::stoi(numericStr);
            };

            return extractNumber(file0) < extractNumber(file1);
        });

        // Collect savegame files.
        auto saveFiles = std::vector<std::filesystem::path>{};
        for (const auto& fileDir : fileDirs)
        {
            for (const auto& saveFile : std::filesystem::directory_iterator(fileDir))
            {
                if (saveFile.is_regular_file() && saveFile.path().extension() == SAVEGAME_FILE_EXT)
                {
                    saveFiles.push_back(saveFile.path());
                }
            }
        }

        // Sort savegame files.
        Sort(saveFiles, [](const std::filesystem::path& file0, const std::filesystem::path& file1)
        {
            auto extractNumber = [](const std::filesystem::path& file)
            {
                auto filename   = file.stem().string(); // "[idx]".
                auto numericStr = std::string();
                for (char curChar : filename)
                {
                    if (std::isdigit(curChar))
                    {
                        numericStr += curChar;
                    }
                }

                return std::stoi(numericStr);
            };

            return extractNumber(file0) < extractNumber(file1);
        });

        // Populate savegame metadata.
        _slotMetadata.clear();
        for (const auto& saveFile : saveFiles)
        {
            auto metadata = GetMetadata(saveFile);
            _slotMetadata.push_back(metadata);
        }
    }

    std::unique_ptr<Savegame> SavegameManager::FromSavegameBuffer(const std::vector<byte>& saveBuffer) const
    {
        // @heapalloc
        auto save = std::make_unique<Savegame>();

        // @todo

        return save;
    }

    std::unique_ptr<std::vector<byte>> SavegameManager::ToSavegameBuffer(const Savegame& save) const
    {
        // @heapalloc
        auto saveBuffer = std::make_unique<std::vector<byte>>();

        // @todo

        return saveBuffer;
    }
}
