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
        int    Unk_0;                     // Same behaviour as `field_0` in `s_SaveSlotElementInfo`.
        uint   GameplayTimer_4;
        ushort SavegameCount_8;
        char   MapEventIdx_A;
        uchar  IsTitleYellowFlag_B_0 : 1;
        uchar  Add290Hours_B_1       : 2;
        uchar  HyperBlasterFlags_B_3 : 5;
    };

    struct SavegameEntry
    {
        int SlotIdx     = 0;
        int SavegameIdx = 0;

        short             Field_0;
        short             SaveCount_2;
        char              ElementType_4;   /** `e_SlotElementType` */
        char              Field_5;         // The value changes between 0 when the first save slot is selected and 4 when the second is selected.
        char              FileIdx_6;
        char              ElementIdx_7;
        int               MapEventIdx_8;
        SavegameMetadata* Metadata_C;
    };

    class SavegameManager
    {
    private:
        // Fields

        Savegame                                       _savegame            = {};
        std::array<SavegameEntry, SAVEGAME_SLOT_COUNT> _slotSavegameEntries = {};

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
        void                  PopulateSlotSavegameLists();

        std::unique_ptr<Savegame>                       FromSavegameBuffer(const Silent::Buffers::Savegame& saveBuffer) const;
        std::unique_ptr<flatbuffers::FlatBufferBuilder> ToSavegameBuffer(const Savegame& save) const;
    };
}
