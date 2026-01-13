#pragma once

namespace Silent::Savegame
{
    constexpr int SAVEGAME_COUNT_MAX = 165; // Max savegames per file.

    /** @brief Savegame info. */
    struct Savegame
    {
        int Dummy = 0;
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
