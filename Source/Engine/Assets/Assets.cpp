#include "Framework.h"
#include "Engine/Assets/Assets.h"

#include "Utils/Parallel.h"

using namespace Silent::Utils;

namespace Silent::Assets
{
    const Asset* AssetManager::GetAsset(const std::string& name) const
    {
        auto it = _assets.find(name);
        if (it != _assets.end())
        {
            return &it->second;
        }

        Log("Attempted to get missing asset '" + name + "'.", LogLevel::Warning, LogMode::Debug);
        return nullptr;
    }

    void AssetManager::Initialize(const std::string& assetsPath)
    {
        _assetsPath = assetsPath;
        auto dirIt  = std::filesystem::recursive_directory_iterator(_assetsPath);

        // Get file count and reserve assets size.
        uint fileCount = 0;
        for (auto it = std::filesystem::recursive_directory_iterator(_assetsPath); it != std::filesystem::recursive_directory_iterator(); it++)
        {
            if (it->is_regular_file())
            {
                fileCount++;
            }
        }
        _assets.reserve(fileCount);

        // Register assets.
        for (const auto& entry : dirIt)
        {
            if (!entry.is_regular_file())
            {
                continue;
            }

            auto name  = entry.path().filename().string();
            auto asset = Asset
            {
                .Name      = name,
                .Extension = entry.path().extension().string(),
                .Path      = entry.path().string(),
                .State     = AssetState::Unloaded,
                .Data      = nullptr
            };
            _assets[asset.Name] = std::move(asset);
        }
    }

    void AssetManager::LoadAsset(const std::string& name)
    {
        // Find asset.
        auto it = _assets.find(name);
        if (it == _assets.end())
        {
            Log("Attempted to load unregistered asset '" + name + "'.", LogLevel::Warning, LogMode::Debug);
            std::cout << "Asset already loaded: " << name << std::endl;
            return;
        }

        // Check if already loading or loaded.
        auto& asset = it->second;
        if (asset.State == AssetState::Loading || asset.State == AssetState::Loaded)
        {
            Log("Attempted to load already loaded asset '" + name + "'.", LogLevel::Warning, LogMode::Debug);
            return;
        }

        // Check if asset path is valid.
        if (!std::filesystem::exists(asset.Path))
        {
            Log("Attempted to load asset '" + asset.Name + " from invalid path: '" + asset.Path + "'.", LogLevel::Error);
            asset.State = AssetState::Error;
            return;
        }

        asset.State = AssetState::Loading;

        // Load asset data to engine object asynchronously.
        g_Parallel.AddTask([&]()
        {
            // TODO

            asset.State = AssetState::Loaded;
        });
    }

    void AssetManager::UnloadAsset(const std::string& name)
    {
        // Find asset.
        auto it = _assets.find(name);
        if (it == _assets.end())
        {
            Log("Attempted to unload unregistered asset '" + name + "'.", LogLevel::Warning, LogMode::Debug);
            return;
        }

        // Check if already unloaded.
        auto& asset = it->second;
        if (asset.State == AssetState::Unloaded)
        {
            Log("Attempted to unload already unloaded asset '" + name + "'.", LogLevel::Warning, LogMode::Debug);
            return;
        }

        // Unload.
        asset.State = AssetState::Unloaded;
        asset.Data.reset();
        Log("Asset '" + name + "' unloaded successfully.", LogLevel::Info, LogMode::Debug);
    }

    void AssetManager::UnloadAllAssets()
    {
        for (auto& [keyName, asset] : _assets)
        {
            asset.State = AssetState::Unloaded;
            asset.Data.reset();
        }

        Log("All assets unloaded.", LogLevel::Info, LogMode::Debug);
    }
}
