#include "Framework.h"
#include "Engine/Services/Savegame.h"

#include "Engine/Services/Configuration.h"

namespace Silent::Services
{
    const std::vector<std::string>& SavegameManager::GetSlotSavegameList(int slotIdx)
    {
        Assert(slotIdx > _slotSavegameLists.size(), "Attempted to get savegame list for invalid slot.");

        return _slotSavegameLists[slotIdx];
    }

    void SavegameManager::Initialize()
    {
        // TODO: Load lists.
    }

    void SavegameManager::Save(int slotIdx, int saveIdx)
    {
        // Create savegame JSON.
        auto saveJson = json{}; // TODO

        // TODO: Parse data from `_savegame` into `savegameJson`.

        // Ensure directory exists.
        auto path = GetSavegamePath(slotIdx, saveIdx);
        std::filesystem::create_directories(path.parent_path());

        // Write savegame file.
        auto outputFile = std::ofstream(path);
        if (outputFile.is_open())
        {
            outputFile << saveJson.dump(ConfigurationManager::JSON_INDENT_SIZE);
            outputFile.close();
        }
    }

    void SavegameManager::Load(int slotIdx, int saveIdx)
    {
        auto path = GetSavegamePath(slotIdx, saveIdx);

        // Open savegame file.
        auto inputFile = std::ifstream(path);
        if (!inputFile.is_open())
        {
            Log("No save " + std::to_string(saveIdx) + " file found for slot " + std::to_string(slotIdx) + ". " + "Creating new save.", LogLevel::Info);

            SetDefaultSavegame();
            Save(slotIdx, saveIdx);
            return;
        }
        
        // Parse file into JSON object.
        auto savegameJson = json();
        inputFile >> savegameJson;

        // TODO: Parse data from `savegameJson` into `_savegame`.
    }

    std::filesystem::path SavegameManager::GetSavegamePath(int slotIdx, int saveIdx) const
    {
        Assert(slotIdx > _slotSavegameLists.size(), "Attempted to get save path for invalid slot.");

        return g_Config.GetAppDir() / ((slotIdx == 0) ? SLOT_1_DIR : SLOT_2_DIR) / (std::to_string(saveIdx) + ".json");
    }

    void SavegameManager::SetDefaultSavegame()
    {
        // TODO
    }

    void SavegameManager::PopulateSlotSavegameLists()
    {
        constexpr char SLOT_DIR_PREFIX[] = "Slot ";
        constexpr char JSON_EXT[]        = ".json";

        auto baseDir = g_Config.GetAppDir();

        for (int i = 0; i < _slotSavegameLists.size(); i++)
        {
            auto paths = std::vector<std::filesystem::path>{};
            
            // Collect JSON file paths.
            auto dir = baseDir / (SLOT_DIR_PREFIX + std::to_string(i + 1));
            for (const auto& entry : std::filesystem::directory_iterator(dir))
            {
                if (entry.is_regular_file() && entry.path().extension() == JSON_EXT)
                {
                    paths.push_back(entry.path());
                }
            }

            // Sort JSON file paths numerically.
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
            auto& saves = _slotSavegameLists[i];
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
}
