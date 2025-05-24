#pragma once

namespace Silent::Assets
{
    enum class AssetState
    {
        Unloaded,
        Loading,
        Loaded,
        Error
    };

    struct Asset
    {
        std::string Name      = {};
        std::string Extension = {};
        std::string Path      = {};

        AssetState            State = AssetState::Unloaded;
        std::shared_ptr<void> Data  = nullptr;
    };

    class AssetManager
    {
    private:
        // Fields

        std::unordered_map<std::string, Asset> _assets     = {}; // Key = name, value = asset.
        std::string                            _assetsPath = {};

    public:
        // Constructors

        AssetManager() = default;

        // Getters

        const Asset* GetAsset(const std::string& name) const;

        // Utilities

        void Initialize(const std::string& assetsPath);

        void LoadAsset(const std::string& name);
        void UnloadAsset(const std::string& name);
        void UnloadAllAssets();
    };
}
