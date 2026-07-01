#include "Framework.h"
#include "Savegame/Savegame.h"

#include "Application.h"
#include "Assets/TranslationKeys.h"
#include "Services/Filesystem.h"
#include "Utils/Bitfield.h"
#include "Utils/Stream.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Savegame
{
    static const auto SAVE_LOCATION_NAME_KEYS = std::vector<std::string>
    {
        KEY_SAVE_LOAD_MENU_LOC_ANYWHERE,
        KEY_SAVE_LOAD_MENU_LOC_CAFE,
        KEY_SAVE_LOAD_MENU_LOC_BUS,
        KEY_SAVE_LOAD_MENU_LOC_STORE,
        KEY_SAVE_LOAD_MENU_LOC_INFIRMARY,
        KEY_SAVE_LOAD_MENU_LOC_DOGHOUSE,
        KEY_SAVE_LOAD_MENU_LOC_GORDON,
        KEY_SAVE_LOAD_MENU_LOC_CHURCH,
        KEY_SAVE_LOAD_MENU_LOC_GARAGE,
        KEY_SAVE_LOAD_MENU_LOC_POLICE,
        KEY_SAVE_LOAD_MENU_LOC_RECEPTION,
        KEY_SAVE_LOAD_MENU_LOC_ROOM_302,
        KEY_SAVE_LOAD_MENU_LOC_DIRECTORS,
        KEY_SAVE_LOAD_MENU_LOC_JEWELLERY_SHOP,
        KEY_SAVE_LOAD_MENU_LOC_POOL_HALL,
        KEY_SAVE_LOAD_MENU_LOC_ANTIQUE_SHOP,
        KEY_SAVE_LOAD_MENU_LOC_THEME_PARK,
        KEY_SAVE_LOAD_MENU_LOC_BOAT,
        KEY_SAVE_LOAD_MENU_LOC_BRIDGE,
        KEY_SAVE_LOAD_MENU_LOC_MOTEL,
        KEY_SAVE_LOAD_MENU_LOC_LIGHTHOUSE,
        KEY_SAVE_LOAD_MENU_LOC_SEWER,
        KEY_SAVE_LOAD_MENU_LOC_NOWHERE,
        KEY_SAVE_LOAD_MENU_LOC_CHILDS_ROOM,
        KEY_SAVE_LOAD_MENU_LOC_NEXT_FEAR
    };

    void SavegameManager::Initialize()
    {
        RefreshMetadata();
    }

    void SavegameManager::Save(int slotIdx, int blockIdx)
    {
        // Ensure slot directory exists.
        auto saveFile = GetSavegameBlockPath(slotIdx, blockIdx);
        stdfs::create_directories(saveFile.parent_path());

        // Write savegame buffer file.
        auto stream = Stream(saveFile, false, true);
        if (stream.IsOpen())
        {
            // @todo
            //stream.WriteArray(ToSpan(*ToSavegameBuffer(_savegame)));
        }

        Debug::Log(Fmt("Saved game to slot {}, block {}.", slotIdx + 1, blockIdx + 1));
    }

    void SavegameManager::Load(int slotIdx, int blockIdx)
    {
        auto saveFile = GetSavegameBlockPath(slotIdx, blockIdx);

        // Open savegame buffer file.
        auto stream = Stream(saveFile, true, false);
        if (!stream.IsOpen())
        {
            Debug::Log(Fmt("Attempted to load missing savegame from slot {}, block {}.", slotIdx + 1, blockIdx + 1),
                       Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return;
        }

        // Get buffer size.
        auto bufferSize = stream.GetSize();

        // Read buffer.
        auto buffer = std::vector<byte>(bufferSize);
        stream.ReadArray(ToSpan(buffer));
        _savegame = std::move(*FromSavegameBuffer(buffer));

        Debug::Log(Fmt("Loaded savegame from slot {}, block {}.", slotIdx + 1, blockIdx + 1));
    }

    const Savegame* SavegameManager::operator->() const
    {
        return &_savegame;
    }

    Savegame* SavegameManager::operator->()
    {
        return &_savegame;
    }

    stdfs::path SavegameManager::GetSavegameBlockPath(int slotIdx, int blockIdx) const
    {
        const auto& fs = g_App.GetFilesystem();

        auto slotName  = SAVEGAME_SLOT_DIR_NAME_BASE + std::to_string(slotIdx + 1);
        auto blockName = std::to_string(blockIdx + 1) + SAVEGAME_FILE_EXT;
        return fs.GetSavegameDirectory() / slotName / blockName;
    }

    SavegameMetadata SavegameManager::GetMetadata(const stdfs::path& saveFile) const
    {
        // Open savegame buffer file.
        auto stream = Stream(saveFile, true, false);
        if (!stream.IsOpen())
        {
            Debug::Log(Fmt("Attempted to get metadata for missing savegame file `{}`.", saveFile.string()),
                       Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return SavegameMetadata
            {
                .DataIdx        = NO_VALUE,
                .SaveCount      = NO_VALUE,
                .LocationId     = NO_VALUE,
                .GameplayTimer  = 0,
                .IsNextFearMode = false,
                .Flags          = NO_VALUE
            };
        }

        // Get buffer size.
        auto bufferSize = stream.GetSize();

        // Read buffer.
        auto buffer = std::vector<byte>(bufferSize);
        stream.ReadArray(ToSpan(buffer));
        auto save = std::move(*FromSavegameBuffer(buffer));
        
        // @todo Read metadata.
        auto metadata = SavegameMetadata{};

        return metadata;
    }

    void SavegameManager::Cleanup()
    {
        const auto& fs = g_App.GetFilesystem();

        auto errorCode = std::error_code();

        // Delete empty slot folders.
        auto emptySlotDirs = std::vector<stdfs::path>{};
        for (const auto& slotDir : stdfs::directory_iterator(fs.GetSavegameDirectory()))
        {
            if (slotDir.is_directory() && stdfs::is_empty(slotDir, errorCode))
            {
                emptySlotDirs.push_back(slotDir.path());
            }
        }
        for (const auto& slotDir : emptySlotDirs)
        {
            stdfs::remove(slotDir, errorCode);
        }

        // Collect valid slot folder paths sorted alphanumerically.
        auto slotDirs = std::vector<stdfs::path>{};
        for (const auto& slotDir : stdfs::directory_iterator(fs.GetSavegameDirectory()))
        {
            if (slotDir.is_directory())
            {
                slotDirs.push_back(slotDir.path());
            }
        }
        Sort(slotDirs);

        // @todo Rename slot folders to follow sequential order.

        // Run through slot folders.
        for (const auto& slotDir : slotDirs)
        {
            // Collect savegame file paths sorted alphanumerically.
            auto saveFiles = std::vector<stdfs::path>{};
            for (const auto& saveFile : stdfs::directory_iterator(slotDir))
            {
                if (saveFile.is_regular_file() && saveFile.path().extension() == SAVEGAME_FILE_EXT)
                {
                    saveFiles.push_back(saveFile.path());
                }
            }
            Sort(saveFiles);

            // Rename savegame files to follow sequential order.
            for (int i = 0; i < saveFiles.size(); i++)
            {
                // Check if filename is already correct.
                auto expectedPath = slotDir / (std::to_string(i + 1) + std::string(SAVEGAME_FILE_EXT));
                if (saveFiles[i] == expectedPath)
                {
                    continue;
                }

                stdfs::rename(saveFiles[i], expectedPath, errorCode);
            }
        }
    }

    void SavegameManager::RefreshMetadata()
    {
        const auto& fs = g_App.GetFilesystem();

        Cleanup();

        // Collect slot folders.
        auto slotDirs = std::vector<stdfs::path>{};
        for (const auto& slotDir : stdfs::directory_iterator(fs.GetSavegameDirectory()))
        {
            if (slotDir.is_directory())
            {
                slotDirs.push_back(slotDir.path());
            }
        }

        // Sort slot folders.
        Sort(slotDirs, [](const stdfs::path& file0, const stdfs::path& file1)
        {
            auto extractNumber = [](const stdfs::path& file)
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

        // @todo Redo this.
        // Collect savegame block files.
        auto blockFiles = std::vector<stdfs::path>{};
        for (const auto& blockDir : slotDirs)
        {
            for (const auto& saveFile : stdfs::directory_iterator(blockDir))
            {
                if (saveFile.is_regular_file() && saveFile.path().extension() == SAVEGAME_FILE_EXT)
                {
                    blockFiles.push_back(saveFile.path());
                }
            }
        }

        // Sort savegame block files.
        Sort(blockFiles, [](const stdfs::path& file0, const stdfs::path& file1)
        {
            auto extractNumber = [](const stdfs::path& file)
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
        _filesMetadata.clear();
        for (int i = 0; i < slotDirs.size(); i++)
        {
            _filesMetadata.push_back({});

            for (const auto& saveFile : blockFiles)
            {
                auto metadata = GetMetadata(saveFile);
                _filesMetadata.back().push_back(metadata);
            }
        }
    }

    std::unique_ptr<Savegame> SavegameManager::FromSavegameBuffer(const std::vector<byte>& saveBuffer) const
    {
        // @heapalloc
        auto save = std::make_unique<Savegame>();

        // Deserialize buffer.
        //auto errorCode = struct_pack::deserialize_to(*save, saveBuffer);
        //if (errorCode != struct_pack::errc::ok)
        //{
        //    Debug::Log("Failed to deserialize savegame buffer.", Debug::LogLevel::Warning);
        //    return nullptr;
        //}

        return save;
    }

    std::unique_ptr<std::vector<byte>> SavegameManager::ToSavegameBuffer(const Savegame& save) const
    {
        // @heapalloc
        auto saveBuffer = std::make_unique<std::vector<byte>>();

        // Serialize buffer.
        //struct_pack::serialize_to(*saveBuffer, save);
        return saveBuffer;
    }
}
