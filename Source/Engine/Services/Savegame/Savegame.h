#pragma once

namespace Silent::Buffers { struct Savegame; }

namespace Silent::Services
{
    constexpr uint SAVEGAME_SLOT_COUNT      = 2;
    constexpr uint SAVEGAME_COUNT_MAX       = 165;
    constexpr uint INVENTORY_ITEM_COUNT_MAX = 40;

    struct Savegame
    {
        int Dummy = 0;
    };
    
    struct SavegameMetadata
    {
        int  SlotIdx        = 0;
        int  FileIdx        = 0;
        int  DataIdx        = 0;
        int  DescriptionIdx = 0;
        int  SaveCount      = 0;
        uint GameplayTimer  = 0;

        bool IsNextFear = false;
        int  Flags      = 0;
    };

    class SavegameManager
    {
    private:
        // Fields

        Savegame                                                       _savegame     = {};
        std::array<std::vector<SavegameMetadata>, SAVEGAME_SLOT_COUNT> _slotMetadata = {};

        std::array<std::vector<std::string>, SAVEGAME_SLOT_COUNT> _slotSavegameLists = {};

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

        std::filesystem::path GetSavegameFilePath(int slotIdx, int saveIdx) const;
        void                  PopulateSlotMetadata();

        SavegameMetadata                                GetMetadata(const std::filesystem::path& saveFile) const;
        std::unique_ptr<Savegame>                       FromSavegameBuffer(const Silent::Buffers::Savegame& saveBuffer) const;
        std::unique_ptr<flatbuffers::FlatBufferBuilder> ToSavegameBuffer(const Savegame& save) const;
    };
}
