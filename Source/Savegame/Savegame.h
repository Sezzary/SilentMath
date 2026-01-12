#pragma once

namespace Silent::Buffers { struct Savegame; }

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

        void Initialize();
        void Save(int fileIdx, int saveIdx);
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

        std::filesystem::path GetSavegamePath(int fileIdx, int saveIdx) const;
        SavegameMetadata      GetMetadata(const std::filesystem::path& saveFile) const;
        
        void                                            PopulateSlotMetadata();
        std::unique_ptr<Savegame>                       FromSavegameBuffer(const Buffers::Savegame& saveBuffer) const;
        std::unique_ptr<flatbuffers::FlatBufferBuilder> ToSavegameBuffer(const Savegame& save) const;
    };
}
