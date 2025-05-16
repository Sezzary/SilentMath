#pragma once

namespace Silent::Services
{
    constexpr int SLOT_COUNT = 2;

    struct Savegame
    {
        // TODO: Good luck.
    };

    class SavegameManager
    {
    private:
        // Constants

        static constexpr char SLOT_1_DIR[] = "Savegame/Slot 1";
        static constexpr char SLOT_2_DIR[] = "Savegame/Slot 2";

        // Fields

        Savegame                                         _savegame          = {};
        std::array<std::vector<std::string>, SLOT_COUNT> _slotSavegameLists = {};

    public:
        // Constructors

        SavegameManager() = default;

        // Getters

        const std::vector<std::string>& GetSlotSavegameList(int slotIdx);

        // Utilities

        void Initialize();
        void Save(int slotIdx, int saveIdx);
        void Load(int slotIdx, int saveIdx);

    private:
        // Helpers

        std::filesystem::path GetSavegamePath(int slotIdx, int saveIdx) const;
        void                  SetDefaultSavegame();
    };
}
