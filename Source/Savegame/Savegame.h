#pragma once

#include "Utils/Bitfield.h"

using namespace Silent::Utils;

namespace Silent::Savegame
{
    constexpr int SAVEGAME_COUNT_MAX = 165; /** Max savegames per file. */

    /** @brief Savegame inventory item entry. */
    struct SavegameInvItem
    {
        int id      = 0; /** `e_InvItemId` */
        int count   = 0;
        int command = 0; /** `e_InvCmdId` */
    };

    /** @brief Savegame info. */
    struct Savegame
    {
        int    SavegameCount  = 0;
        int    LocationId     = 0; /** `e_SaveLocationId` */
        q20_12 GameplayTimer  = Q12(0.0f);
        bool   IsNextFearMode = false;

        int      ContinueCount = 0;
        int      MapIdx        = 0; /** `e_MapIdx` */
        int      MapRoomIdx    = 0; /** Index to local map geometry `.IPD` files. */
        int      PaperMapIdx   = 0; /** `e_PaperMapIdx` | Index of the paper map displayed in the map screen. */
        int      PaperMapFlags = 0;
        Bitfield EventFlags    = Bitfield(1664);
        int      MapEnemyStates[45]; /** Flags indicating enemy states in the map. */

        q19_12 PlayerPositionX  = Q12(0.0f);
        q19_12 PlayerPositionZ  = Q12(0.0f);
        q3_12  PlayerRotationY  = Q12_ANGLE(0.0f);
        q19_12 PlayerHealth     = Q12(0.0f);
        q19_12 HealthSaturation = Q12(0.0f);
        q20_12 RunDistance      = Q12(0.0f);
        q20_12 WalkDistance     = Q12(0.0f);

        std::vector<SavegameInvItem> Items                    = {};
        int                          InventorySlotCount       = 0; /** Item slots. */
        int                          PickedUpItemCount        = 0;
        int                          PickedUpSpecialItemCount = 0;
        int                          InventoryItemFlags       = 0; /** `e_InvItemFlags` */
        int                          ItemToggleFlags          = 0; /** `e_ItemToggleFlags` */
        int                          EquippedWeapon           = 0; /** `e_InvItemId` | Affects the visible player weapon model. */

        int GameDifficulty      = 0; /** `e_GameDifficulty` */
        int MeleeKillCount      = 0;
        int MeleeKillCountB     = 0;
        int RangedKillCount     = 0;
        int firedShotCount      = 0;
        int CloseRangeShotCount = 0;
        int MidRangeShotCount   = 0;
        int LongRangeShotCount  = 0;
        int ClearGameCount      = 0;
        int ClearGameEndings    = 0; /** `e_GameEndingFlags` */
        u8  Add290Hours : 2     = 0; /** Adds 290 hours per 1 bit, i.e. 290, 580, 870. */
    };

    /** @brief Savegame metadata to display. */
    struct SavegameMetadata
    {
        int  DataIdx        = 0;
        int  SaveCount      = 0;
        int  LocationId     = 0;
        int  GameplayTimer  = 0;
        bool IsNextFearMode = false;
        int  Flags          = 0;
    };

    /** @brief Savegame manager. */
    class SavegameManager
    {
    private:
        // =======
        // Fields
        // =======

        Savegame                                   _savegame      = {};
        std::vector<std::vector<SavegameMetadata>> _filesMetadata = {};

    public:
        // =============
        // Constructors
        // =============

        /** @brief Constructs a default uninitialized instance. */
        SavegameManager() = default;

        // ==========
        // Utilities
        // ==========

        /** @brief Initializes the `SavegameManager`. */
        void Initialize();

        /** @brief Saves the active savegame data to a block on the disk.
         *
         * @param slotIdx Index of the slot containing savegame blocks.
         * @param blockIdx Index of the savegame block in the slot.
         */
        void Save(int slotIdx, int blockIdx);

        /** @brief Loads active savegame data from a block on the disk.
         *
         * @param slotIdx Index of the slot containing savegame blocks.
         * @param blockIdx Index of the savegame block in the slot.
         */
        void Load(int slotIdx, int blockIdx);

        // ==========
        // Operators
        // ==========

        const Savegame* operator->() const;
        Savegame*       operator->();

    private:
        // ========
        // Helpers
        // ========

        /** @brief Gets the path to a savegame block on the disk.
         *
         * @param slotIdx Index of the slot containing savegame blocks.
         * @param blockIdx Index of the savegame block in the slot.
         * @return Savegame block path.
         */
        stdfs::path GetSavegameBlockPath(int slotIdx, int blockIdx) const;

        /** @brief Gets slot metadata from a savegame file.
         *
         * @param saveFile Savegame buffer file path.
         * @return Savegame file metadata.
         */
        SavegameMetadata GetMetadata(const stdfs::path& saveFile) const;

        /** @brief Organizes the savegame file structure on the disk. */
        void Cleanup();

        /** @brief Repopulates metadata for all savegame blocks. */
        void RefreshMetadata();

        /** @brief Converts a serialized savegame buffer to an engine savegame.
         *
         * @param saveBuffer Serialized savegame buffer.
         * @return Engine savegame.
         */
        std::unique_ptr<Savegame> FromSavegameBuffer(const std::vector<byte>& saveBuffer) const;

        /** @brief Converts an engine savegame to a serialized savegame buffer.
         *
         * @param save Engine savegame.
         * @return Serialized savegame buffer.
         */
        std::unique_ptr<std::vector<byte>> ToSavegameBuffer(const Savegame& save) const;
    };
}
