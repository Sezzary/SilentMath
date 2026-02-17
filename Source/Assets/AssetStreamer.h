#pragma once

#include "Assets/Parsers/Ilm.h"
#include "Assets/Parsers/Ipd.h"
#include "Assets/Parsers/Plm.h"
#include "Assets/Parsers/Png.h"
#include "Assets/Parsers/Tim.h"
#include "Assets/Parsers/Tmd.h"

namespace Silent::Assets
{
    /** @brief Streamable asset types. Used in `Asset`. */
    enum class AssetType
    {
        Tim, /** "Texture IMage"                 | PsyQ SDK texture data. */
        Vab, /** "Voice Audio Bank"              | PsyQ SDK audio container data. */
        Bin, /** "BINary"                        | Original compiled logic overlay data. */
        Dms, /** "Demo Motion Sequence"?         | Cutscene keyframe data. */
        Anm, /** "ANiMation"                     | Animation data. */
        Plm, /** "PoLygon Model"?                | Model data. */
        Ipd, /** "Instanced Polygon model Data"? | Map model and collision data. Used for environment streaming. */
        Ilm, /** "Instanced Linked Model"?       | Skeletal model data. Used for characters. */
        Tmd, /** "Three-dimensional Model Data"  | PsyQ SDK 3D model data. Used for inventory items. */
        Dat, /** "Demo dATa"?                    | Demo playback data. */
        Kdt, /** "Key Data Tracker"?             | Konami MIDI tracker data. */
        Cmp, /** "CoMPressed" or "CoMPiled"?     | Unknown. */
        Xa,  /** "eXtended Audio"                | PSX XA audio stream. */

        Png
    };

    /** @brief Streamable asset states. Used in `Asset`. */
    enum class AssetState
    {
        Unloaded,
        Loading,
        Loaded,
        Error
    };

    /** @brief Streamable asset. */
    struct Asset
    {
        std::string             Name = {};                    /** Filename relative to assets folder. */
        AssetType               Type = AssetType::Tim;        /** File type. */
        std::filesystem::path   File = {};                    /** Absolute system file path. */
        uint64                  Size = 0;                     /** Raw file size in bytes. */
        std::atomic<AssetState> State = AssetState::Unloaded; /** Thread-safe load state. */
        std::shared_ptr<void>   Data  = nullptr;              /** Thread-safe parsed data. */

        /** @brief Gets the typed asset data. The asset must be loaded before calling.
         *
         * @tparam T Loaded asset type to cast the asset data to.
         * @return Typed loaded asset data.
         * @throws `std::runtime_error` if `Data` is `nullptr`.
         */
        template <typename T>
        std::shared_ptr<const T> GetData() const
        {
            if (Data == nullptr)
            {
                throw std::runtime_error("Attempted to get data for unloaded asset.");
            }

            return std::reinterpret_pointer_cast<T>(Data);
        }
    };

    /** @brief Asset streamer. */
    class AssetStreamer
    {
    private:
        // =======
        // Fields
        // =======

        std::vector<std::unique_ptr<Asset>>        _assets       = {}; /** Registered assets. */
        std::unordered_map<int, std::string>       _names        = {}; /** Key = asset index, value = asset name. */
        std::unordered_map<std::string, int>       _idxs         = {}; /** Key = asset name, value = asset index. */
        std::unordered_map<int, std::future<void>> _loadFutures  = {}; /** Key = asset index, value = load future. */
        std::atomic<int>                           _loadingCount = 0;  /** Number of currently loading assets. */

    public:
        // =============
        // Constructors
        // =============

        AssetStreamer() = default;

        // ========
        // Getters
        // ========

        /** Gets an asset's name by its legacy index.
         *
         * @param assetIdx Legacy asset file index.
         * @return Asset name.
         */
        const std::string& GetName(int assetIdx) const;

        /** @brief Gets a vector containing the names of all loaded assets.
         *
         * @return Vector of all loaded asset names.
         */
        std::vector<std::string> GetLoadedNames() const;

        /** @brief Gets a loaded asset via a file index.
         *
         * @param assetIdx Asset file index.
         * @return Pointer to an `Asset` object if the asset is loaded, `nullptr` otherwise.
         */
        const Asset* GetAsset(int assetIdx);

        /** @brief Gets a loaded asset via a filename.
         *
         * @param assetName Asset filename.
         * @return Pointer to an `Asset` object if the asset is loaded, `nullptr` otherwise.
         */
        const Asset* GetAsset(const std::string& assetName);

        // ==========
        // Inquirers
        // ==========

        /** @brief Checks if assets are currently being loaded.
         *
         * @return Busy status.
         */
        bool IsBusy() const;

        // ==========
        // Utilities
        // ==========

        /** @brief Initializes the asset manager.
         *
         * @param assetsPath Assets folder path on the system.
         */
        void Initialize(const std::filesystem::path& assetsPath);

        /** @brief Loads an asset by index.
         *
         * @param assetIdx Index of the asset to load.
         * @return `std::future` of the asset's load status.
         */
        const std::future<void>& Load(int assetIdx);

        /** @brief Loads an asset by name.
         *
         * @param assetName Name of the asset to load.
         * @return `std::future` of the asset's load status.
         */
        const std::future<void>& Load(const std::string& assetName);

        /** @brief Unloads an asset by index.
         *
         * @param assetIdx Index of the asset to unload.
         */
        void Unload(int assetIdx);

        /** @brief Unloads an asset by name.
         *
         * @param assetName Name of the asset to unload.
         */
        void Unload(const std::string& assetName);

        /** @brief Unloads all currently loaded assets. */
        void UnloadAll();
    };
}
