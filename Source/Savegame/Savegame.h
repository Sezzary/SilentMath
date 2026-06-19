#pragma once

#include "Utils/Bitfield.h"

using namespace Silent::Utils;

namespace Silent::Savegame
{
    constexpr int SAVEGAME_COUNT_MAX = 165; /** Max savegames per file. */

    /** @brief Savegame inventory item entry. */
    struct SavegameInvItem
    {
        int id;      /** `e_InvItemId` */
        int count;
        int command; /** `e_InvCmdId` */
    };

    /** @brief Savegame info. */
    struct Savegame
    {
        std::vector<SavegameInvItem> items = {};
        int             mapIdx;                      /** `e_MapIdx` */
        int             mapRoomIdx;                  /** Index to local map geometry `.IPD` files. */
        int             savegameCount;
        int             locationId;                  /** `e_SaveLocationId` */
        int             paperMapIdx;                 /** `e_PaperMapIdx` | Index of the paper map displayed when opening the map screen. */
        int             equippedWeapon;              /** `e_InvItemId` | Affects the visible player weapon model. */
        int             inventorySlotCount;          /** Item slots. */
        int             itemToggleFlags;             /** `e_ItemToggleFlags` */
        int             ovlEnemyStates[45];          /** Flags indicating the enemy states in a given overlay.
                                                     * All set to 1 by default. As soon as they are killed (not just stunned),
                                                     * set to 0 based on a currently unknown index value.
                                                     */
        int             paperMapFlags;               // See Sparagas' `HasMapsFlags` struct for details of every bit.
        Bitfield        eventFlags = Bitfield(1664);
        q19_12          healthSaturation;            /** Range: [0, 300]. Ampoules give extra stored health. If the player loses health, it will be slowly restored. */
        int             pickedUpItemCount;
        int             inventoryItemFlags; /** `e_InventoryItemFlags` */
        q19_12          playerHealth;       /** Default: `Q12(100.0f)` */
        q19_12          playerPositionX;
        q3_12           playerRotationY;  /** Range [0, 0.999755859375], positive Z: 0, clockwise rotation. It can be multiplied by 360 to get degrees. */
        int             clearGameCount;   /** Range [0, 99] */
        int             clearGameEndings; /** `e_GameEndingFlags` */
        q19_12          playerPositionZ;
        q20_12          gameplayTimer;
        q20_12          runDistance;
        q20_12          walkDistance;
        bool            isNextFearMode;
        u8              add290Hours : 2; /** Adds 290 hours per 1 bit, i.e. 290, 580, 870. */
        int             pickedUpSpecialItemCount; 
        int             meleeKillCount;
        int             meleeKillCountB;
        int             rangedKillCount;
        int             gameDifficulty; /** `e_GameDifficulty` */
        int             firedShotCount;
        int             closeRangeShotCount;
        int             midRangeShotCount;
        int             longRangeShotCount;
        int             continueCount;
    };

    /** @brief Savegame metadata. */
    struct SavegameMetadata
    {
        int FileIdx       = 0;
        int DataIdx       = 0;
        int SaveCount     = 0;
        int LocationId    = 0;
        int GameplayTimer = 0;

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

        Savegame                      _savegame     = {};
        std::vector<SavegameMetadata> _slotMetadata = {};

    public:
        // =============
        // Constructors
        // =============

        SavegameManager() = default;

        // ========
        // Getters
        // ========

        const std::vector<SavegameMetadata>& GetSlotMetadata();

        // ==========
        // Utilities
        // ==========

        /** @brief Initializes the `SavegameManager`. */
        void Initialize();

        /** @brief Saves the active savegame to a file.
         *
         * @param fileIdx Index of a file containing savegames.
         * @param saveIdx Index of a savegame within the file.
         */
        void Save(int fileIdx, int saveIdx);

        /** @brief Loads an active savegame from a file.
         *
         * @param fileIdx Index of a file containing savegames.
         * @param saveIdx Index of a savegame within the file.
         */
        void Load(int fileIdx, int saveIdx);

        // ==========
        // Operators
        // ==========

        const Savegame* operator->() const;
        Savegame*       operator->();

    private:
        // ========
        // Helpers
        // ========

        /** @brief Gets the path of a savegame file.
         *
         * @param fileIdx Index of a file containing savegames.
         * @param saveIdx Index of a savegame within the file.
         * @return Savegame path.
         */
        std::filesystem::path GetSavegamePath(int fileIdx, int saveIdx) const;

        SavegameMetadata GetMetadata(const std::filesystem::path& saveFile) const;
        
        void PopulateSlotMetadata();

        /** @brief Converts a savegame serialized buffer to a savegame.
         *
         * @param saveBuffer Serialized savegame buffer.
         * @return Savegame.
         */
        std::unique_ptr<Savegame> FromSavegameBuffer(const std::vector<byte>& saveBuffer) const;

        /** @brief Converts a savegame to a serialized savegame buffer.
         *
         * @param save Savegame.
         * @return Serialized savegame buffer.
         */
        std::unique_ptr<std::vector<byte>> ToSavegameBuffer(const Savegame& save) const;
    };
}
