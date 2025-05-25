#include "Framework.h"
#include "Engine/Services/Savegame/Savegame.h"

#include "Engine/Application.h"
#include "Engine/Services/Configuration.h"
#include "Engine/Services/Savegame/Generated/savegame_generated.h"

namespace Silent::Services
{
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
        // TODO: Load lists, create empty paths.
    }

    void SavegameManager::Save(int slotIdx, int saveIdx)
    {
        Log("Saving game to slot " + std::to_string(slotIdx + 1) + ", savegame " + std::to_string(saveIdx + 1) + ".", LogLevel::Info);

        // Create savegame buffer.
        auto saveBuffer = ToSavegameBuffer(_savegame);

        // Ensure directory exists.
        auto path = GetSavegameFilePath(slotIdx, saveIdx);
        std::filesystem::create_directories(path.parent_path());

        // Write savegame buffer.
        auto outputFile = std::ofstream(path);
        if (outputFile.is_open())
        {
            outputFile.write((const char*)saveBuffer->GetBufferPointer(), saveBuffer->GetSize());
            outputFile.close();
        }
    }

    void SavegameManager::Load(int slotIdx, int saveIdx)
    {
        auto path = GetSavegameFilePath(slotIdx, saveIdx);

        // Open savegame buffer file.
        auto inputFile = std::ifstream(path, std::ios::binary);
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
        return config.GetWorkFolderPath() / SAVEGAME_FOLDER_NAME / slotFolderName / saveFilename;
    }

    // TODO: Will probably need a completely different approach when we know how the save system works.
    void SavegameManager::PopulateSlotSavegameLists()
    {
        const auto& config = g_App.GetConfig();

        for (int slotIdx = 0; slotIdx < _slotSavegameLists.size(); slotIdx++)
        {
            auto paths = std::vector<std::filesystem::path>{};
            
            auto slotFolderName = SAVEGAME_SLOT_FOLDER_NAME_BASE + std::to_string(slotIdx + 1);
            auto path           = config.GetWorkFolderPath() / slotFolderName;

            // Collect savegame file paths.
            for (const auto& entry : std::filesystem::directory_iterator(path))
            {
                if (entry.is_regular_file() && entry.path().extension() == SAVEGAME_FILE_EXT)
                {
                    paths.push_back(entry.path());
                }
            }

            // Sort savegame file paths numerically.
            std::sort(paths.begin(), paths.end(), [](const std::filesystem::path& path0, const std::filesystem::path& path1)
            {
                auto extractNumber = [](const std::filesystem::path& path)
                {
                    auto filename   = path.stem().string(); // "Save[idx]".
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

                return extractNumber(path0) < extractNumber(path1);
            });

            // Populate savegame list.
            auto& saves = _slotSavegameLists[slotIdx];
            saves.clear();
            for (const auto& path : paths)
            {
                auto inputFile = std::ifstream(path);
                if (!inputFile.is_open())
                {
                    saves.push_back("CORRUPTED SAVE");
                    continue;
                }

                auto saveJson = json();
                inputFile >> saveJson;

                // TODO: Figure out how description is actually going to be retrieved.
                const auto& desc = saveJson.value("Description", "Description Unimplemented");
                saves.push_back(desc);
            }
        }
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
