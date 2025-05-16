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
        auto path = GetSavegamePath(slotIdx, saveIdx);

        // Create savegame JSON.
        auto savegameJson = json{};

        // TODO: Parse data from `_savegame` into `savegameJson`.
        
        // Ensure directory exists.
        std::filesystem::create_directories(path.parent_path());

        // Write savegame file.
        auto outputFile = std::ofstream(path);
        if (outputFile.is_open())
        {
            outputFile << savegameJson.dump(ConfigurationManager::JSON_INDENT_SIZE);
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
}
