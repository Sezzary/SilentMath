#include "Framework.h"
#include "Engine/Services/Savegame/Savegame.h"

#include "Engine/Application.h"
#include "Engine/Services/Configuration.h"
#include "Engine/Services/Savegame/Generated/savegame_generated.h"

namespace Silent::Services
{
    static const char* SAVEGAME_DESC_STRINGS[] =
    {
        "Anywhere",
        "Cafe",
        "Bus",
        "Store",
        "Infirmary",
        "Doghouse",
        "Gordon",
        "Church",
        "Garage",
        "Police",
        "Reception",
        "Room_302",
        "Director's",
        "Jewelry_shop",
        "Pool_hall",
        "Antique_shop",
        "Theme_park",
        "Boat",
        "Bridge",
        "Motel",
        "Lighthouse",
        "Sewer",
        "Nowhere",
        "Child's_room",
        "Next_fear"
    };

    const std::vector<std::string>& SavegameManager::GetSlotSavegameList(int slotIdx)
    {
        if (slotIdx > _slotSavegameLists.size())
        {
            throw std::invalid_argument("Attempted to get savegame list for invalid slot.");
        }

        return _slotSavegameLists[slotIdx];
    }

    void SavegameManager::Initialize()
    {
        PopulateSlotMetadata();
    }

    void SavegameManager::Save(int slotIdx, int saveIdx)
    {
        Log("Saving game to slot " + std::to_string(slotIdx + 1) + ", savegame " + std::to_string(saveIdx + 1) + ".", LogLevel::Info);

        // Create savegame buffer.
        auto saveBuffer = ToSavegameBuffer(_savegame);

        // TODO: Fill metadata.

        // Ensure directory exists.
        auto saveFile = GetSavegameFilePath(slotIdx, saveIdx);
        std::filesystem::create_directories(saveFile.parent_path());

        // Write savegame buffer.
        auto outputFile = std::ofstream(saveFile);
        if (outputFile.is_open())
        {
            outputFile.write((const char*)saveBuffer->GetBufferPointer(), saveBuffer->GetSize());
            outputFile.close();
        }
    }

    void SavegameManager::Load(int slotIdx, int saveIdx)
    {
        auto saveFile = GetSavegameFilePath(slotIdx, saveIdx);

        // Open savegame buffer file.
        auto inputFile = std::ifstream(saveFile, std::ios::binary);
        if (!inputFile.is_open())
        {
            Log("Attempted to load missing savegame file for slot " + std::to_string(slotIdx + 1) + ", save " + std::to_string(saveIdx + 1) + ".",
                LogLevel::Warning, LogMode::Debug);
            return;
        }

        // Get file size.
        inputFile.seekg(0, std::ios::end);
        auto fileSize = inputFile.tellg();
        inputFile.seekg(0, std::ios::beg);

        // Read file into buffer object.
        auto fileBuffer = std::vector<char>(fileSize);
        inputFile.read(fileBuffer.data(), fileSize);
        auto* saveBuffer = flatbuffers::GetRoot<Silent::Buffers::Savegame>(fileBuffer.data());

        // Read savegame buffer.
        _savegame = std::move(*FromSavegameBuffer(*saveBuffer));
    }

    std::filesystem::path SavegameManager::GetSavegameFilePath(int slotIdx, int saveIdx) const
    {
        if (slotIdx >= _slotSavegameLists.size())
        {
            throw std::invalid_argument("Attempted to get savegame file path for invalid slot.");
        }

        const auto& config = g_App.GetConfig();

        auto slotFolderName = SAVEGAME_SLOT_FOLDER_NAME_BASE + std::to_string(slotIdx + 1);
        auto saveFilename   = std::to_string(saveIdx + 1) + SAVEGAME_FILE_EXT;
        return config.GetWorkFolder() / SAVEGAME_FOLDER_NAME / slotFolderName / saveFilename;
    }

    void SavegameManager::PopulateSlotMetadata()
    {
        const auto& config = g_App.GetConfig();

        for (int slotIdx = 0; slotIdx < _slotMetadata.size(); slotIdx++)
        {
            auto slotFolderName = SAVEGAME_SLOT_FOLDER_NAME_BASE + std::to_string(slotIdx + 1);
            auto slotFolder     = config.GetWorkFolder() / slotFolderName;
            if (!std::filesystem::exists(slotFolder) || !std::filesystem::is_directory(slotFolder))
            {
                continue;
            }
            
            // Collect savegame files.
            auto saveFiles = std::vector<std::filesystem::path>{};
            for (const auto& entry : std::filesystem::directory_iterator(slotFolder))
            {
                if (entry.is_regular_file() && entry.path().extension() == SAVEGAME_FILE_EXT)
                {
                    saveFiles.push_back(entry.path());
                }
            }

            // Sort savegame files numerically.
            std::sort(saveFiles.begin(), saveFiles.end(), [](const std::filesystem::path& file0, const std::filesystem::path& file1)
            {
                auto extractNumber = [](const std::filesystem::path& file)
                {
                    auto filename   = file.stem().string(); // "Save[idx]".
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

            // Clear previous metadata.
            auto& slotMetadata = _slotMetadata[slotIdx];
            slotMetadata.clear();

            // Populate savegame metadata.
            for (const auto& saveFile : saveFiles)
            {
                auto metadata = GetMetadata(saveFile);
                slotMetadata.push_back(metadata);
            }
        }
    }

    SavegameMetadata SavegameManager::GetMetadata(const std::filesystem::path& saveFile) const
    {
        // Open savegame buffer file.
        auto inputFile = std::ifstream(saveFile, std::ios::binary);
        if (!inputFile.is_open())
        {
            Log("Attempted to get metadata for missing savegame file `" + saveFile.string() + "'.", LogLevel::Warning, LogMode::Debug);
            return SavegameMetadata
            {
                .SlotIdx        = NO_VALUE,
                .FileIdx        = NO_VALUE,
                .DataIdx        = NO_VALUE,
                .DescriptionIdx = NO_VALUE,
                .SaveCount      = NO_VALUE,
                .GameplayTimer  = 0,
                .IsNextFear     = false,
                .Flags          = NO_VALUE
            };
        }

        // Get file size.
        inputFile.seekg(0, std::ios::end);
        auto fileSize = inputFile.tellg();
        inputFile.seekg(0, std::ios::beg);

        // Read file into buffer object.
        auto fileBuffer = std::vector<char>(fileSize);
        inputFile.read(fileBuffer.data(), fileSize);
        auto* saveBuffer = flatbuffers::GetRoot<Silent::Buffers::Savegame>(fileBuffer.data());
        
        // TODO: Read metadata from savegame buffer.
        auto metadata = SavegameMetadata{};

        return metadata;
    }

    std::unique_ptr<Savegame> SavegameManager::FromSavegameBuffer(const Silent::Buffers::Savegame& saveBuffer) const
    {
        auto save = std::make_unique<Savegame>();

        // TODO

        return save;
    }

    std::unique_ptr<flatbuffers::FlatBufferBuilder> SavegameManager::ToSavegameBuffer(const Savegame& save) const
    {
        auto saveBuffer = std::make_unique<flatbuffers::FlatBufferBuilder>();

        // TODO

        return saveBuffer;
    }
}
