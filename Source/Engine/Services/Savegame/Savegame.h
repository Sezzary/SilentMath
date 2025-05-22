#pragma once

namespace Silent::Buffers { struct Savegame; }

namespace Silent::Services
{
    constexpr int SLOT_COUNT               = 2;
    constexpr int INVENTORY_ITEM_COUNT_MAX = 40;

    enum class GameDifficulty
    {
        Easy   = -1,
        Normal = 0,
        Hard   = 1
    };

    struct InventoryItem
    {
        u8 Id;      /** `InventoryItemId` */
        u8 Count;
        u8 Command; /** `InventoryCommandId` */
        u8 Unk_3;   // Some sort of index?
    };

    struct Savegame
    {
        std::string Description = {};

        InventoryItem   Items[INVENTORY_ITEM_COUNT_MAX];
        s8                field_A0;
        s8                field_A1[3];
        char                MapOverlayIdx;         /** Index to overlay `*.BIN` files. */
        char                MapRoomIdx;            /** Index to local map geometry `*.IPD` files. */
        short               SaveGameCount;
        s8                MapEventIdx;          // See Sparagas' `SaveTitle` enum for details of every value.
        u8                MapIdx_A9;                /** Index to global map geometry `*.PLM` files. */
        s8                EquippedWeapon_AA;        /** `InventoryItemId` */
        u8                field_AB;
        u32               flags_AC;
        s32               field_B0[45];

        s32 HasMapsFlags_164;         // See Sparagas' `HasMapsFlags` struct for details of every bit.
        s32 EventFlags_168[6];        //----------------------------------------
        s32 EventFlags_180[2];        //
        s32 EventFlags_188;           //
        s32 EventFlags_18C;           // Only tested a few, but it seems all are related to events and pick-up flags, grouped by location and not item types.
        s32 EventFlags_190[4];        //
        s32 EventFlags_1A0;           //
        s32 EventFlags_1A4[12];       //----------------------------------------
        s32 MapMarkingFlags_1D4[2];   //----------------------------------------
        s32 MapMarkingFlags_1DC;      // These 3 are one `u32 mapMarkingFlags[25];` (or maybe `u8 mapMarkingFlags[100];`?) See Sparagas' `MapMarkingsFlags` struct for details of every bit.
        s32 MapMarkingFlags_1E0[22];  //----------------------------------------

        s32               field_238;
        short               PickedUpItemCount;
        s8                field_23E;
        s8                field_23F;
        int            PlayerHealth;
        int            PlayerPositionX;
        short             PlayerRotationY;
        u8                field_24A;
        u8                field_24B;
        int            PlayerPositionZ;
        int            GameplayTimer;
        int            RunDistance;
        int            WalkDistance;
        s32               EnemyKillCountPacked; // Redo to `rangedKillCount : 8; meleeKillCount : 16; pad : 8` or `u8 pad; u16 meleeKillCount; s8 rangedKillCount;`.
        s32               field_260 : 28;
        GameDifficulty GameDifficulty;
        s16               FiredShotCount;
        s16               CloseRangeShotCount;
        s16               MidRangeShotCount;
        s16               LongRangeShotCount;
        s16               field_26C;
        s16               field_26E;                // Related to enemy kills.
        s16               field_270;
        s16               field_272;
        s16               field_274;
        s16               field_276;
        s16               field_278;
        s8                field_27A;
        uchar                ContinueCount;
    };

    class SavegameManager
    {
    private:
        // Constants

        static constexpr char SLOT_FOLDER_PATH_BASE[] = "Savegame/Slot ";
        static constexpr char SAVEGAME_FILE_EXT[]     = ".save";

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

        std::filesystem::path GetSavegameFilePath(int slotIdx, int saveIdx) const;
        void                  PopulateSlotSavegameLists();

        std::unique_ptr<Savegame>                       FromSavegameBuffer(const Silent::Buffers::Savegame& saveBuffer) const;
        std::unique_ptr<flatbuffers::FlatBufferBuilder> ToSavegameBuffer(const Savegame& save) const;
    };
}
