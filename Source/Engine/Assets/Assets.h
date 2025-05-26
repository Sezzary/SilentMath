#pragma once

namespace Silent::Assets
{
    enum class AssetType
    {
        Tim, // PsyQ SDK texture data.
        Vab, // PsyQ SDK audio container data.
        Bin, // Original compiled logic overlay data.
        Dms, // Cutscene keyframe data.
        Anm, // Animation data.
        Plm, // Global static model data.
        Ipd, // Local static model data.
        Ilm, // Skeletal model data.
        Tmd, // PsyQ SDK 3D model data. Used exclusively for inventory.
        Dat, // Demo playback data.
        Kdt, // Konami MIDI tracker data.
        Cmp, // Unknown. "Compressed"?
        Xa,  // Audio data.

        Count
    };

    enum class AssetState
    {
        Unloaded,
        Loading,
        Loaded,
        Error
    };

    struct Asset
    {
        AssetType             Type = AssetType::Tim; // File extension type.
        std::filesystem::path File = {};             // Absolute file path.
        uint64                Size = 0;              // Raw file size in bytes.
        
        std::atomic<AssetState> State = AssetState::Unloaded; // Thread-safe load state.
        std::shared_ptr<void>   Data  = nullptr;              // Parsed engine object data.
    };

    class AssetManager
    {
    private:
        // Fields

        std::vector<std::shared_ptr<Asset>> _assets    = {};
        std::atomic<uint>                   _busyCount = 0;

    public:
        // Constructors

        AssetManager() = default;

        // Getters

        const std::shared_ptr<Asset> GetAsset(int assetIdx) const;

        // Inquirers

        bool IsBusy() const;

        // Utilities

        void Initialize(const std::filesystem::path& assetsPath);

        void LoadAsset(int assetIdx);
        void UnloadAsset(int assetIdx);
        void UnloadAllAssets();
    };
}
