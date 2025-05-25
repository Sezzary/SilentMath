#pragma once

namespace Silent::Assets
{
    enum class AssetType
    {
        Tim,
        Vab,
        Bin,
        Dms,
        Anm,
        Plm,
        Ipd,
        Ilm,
        Tmd,
        Dat,
        Kdt,
        Cmp
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
        std::filesystem::path File = {};             // Absolute asset file path.
        uint64                Size = 0;              // Raw file size in bytes.
        
        AssetState            State = AssetState::Unloaded; // NOT thread-safe load state.
        std::shared_ptr<void> Data  = nullptr;              // Parsed engine object data.
    };

    class AssetManager
    {
    private:
        // Fields

        std::vector<Asset> _assets = {}; // Index = asset ID.

    public:
        // Constructors

        AssetManager() = default;

        // Getters

        const Asset* GetAsset(int assetIdx) const;

        // Utilities

        void Initialize(const std::filesystem::path& assetsPath);

        void LoadAsset(int assetIdx);
        void UnloadAsset(int assetIdx);
        void UnloadAllAssets();
    };
}
