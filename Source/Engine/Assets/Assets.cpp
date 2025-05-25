#include "Framework.h"
#include "Engine/Assets/Assets.h"

#include "Utils/Parallel.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Assets
{
    static const auto ASSET_TYPES = std::unordered_map<std::string, AssetType>
    {
        { ".TIM", AssetType::Tim },
        { ".VAB", AssetType::Vab },
        { ".BIN", AssetType::Bin },
        { ".DMS", AssetType::Dms },
        { ".ANM", AssetType::Anm },
        { ".PLM", AssetType::Plm },
        { ".IPD", AssetType::Ipd },
        { ".ILM", AssetType::Ilm },
        { ".TMD", AssetType::Tmd },
        { ".DAT", AssetType::Dat },
        { ".KDT", AssetType::Kdt },
        { ".CMP", AssetType::Cmp }
    };

    const Asset* AssetManager::GetAsset(int assetIdx) const
    {
        // Get asset.
        if (assetIdx < 0 || assetIdx >= _assets.size())
        {
            Log("Attempted to get asset with invalid index " + std::to_string(assetIdx) + ".", LogLevel::Warning, LogMode::Debug);
            return nullptr;
        }
        const auto& asset = _assets[assetIdx];

        return &asset;
    }

    void AssetManager::Initialize(const std::filesystem::path& assetsPath)
    {
        // Collect files.
        auto files = std::vector<std::filesystem::path>{};
        for (auto& entry : std::filesystem::recursive_directory_iterator(assetsPath))
        {
            if (entry.is_regular_file())
            {
                files.push_back(entry.path());
            }
        }

        // Sort files alphabetically.
        std::sort(files.begin(), files.end());
        
        // Register assets.
        _assets.reserve(files.size());
        for (int idx = 0; idx < files.size(); idx++)
        {
            const auto& file = files[idx];

            // Check if type is known.
            auto ext = ToUpper(file.extension().string());
            if (ASSET_TYPES.find(ext) == ASSET_TYPES.end())
            {
                Log("Unknown type for asset file '" + file.string() + "'.", LogLevel::Warning, LogMode::Debug);
                continue;
            }

            // Add asset.
            auto asset = Asset
            {
                .Type  = ASSET_TYPES.at(ext),
                .File  = file,
                .Size  = std::filesystem::file_size(file),
                .State = AssetState::Unloaded,
                .Data  = nullptr
            };
            _assets.push_back(std::move(asset));
        }
    }

    void AssetManager::LoadAsset(int assetIdx)
    {
        // Get asset.
        if (assetIdx < 0 || assetIdx >= _assets.size())
        {
            Log("Attempted to load invalid asset " + std::to_string(assetIdx) + ".", LogLevel::Warning, LogMode::Debug);
            return;
        }
        auto& asset = _assets[assetIdx];

        // Check if already loading or loaded.
        if (asset.State == AssetState::Loading || asset.State == AssetState::Loaded)
        {
            Log("Attempted to load already loading/loaded asset " + std::to_string(assetIdx) + ".", LogLevel::Warning, LogMode::Debug);
            return;
        }

        // Check if file is valid.
        if (!std::filesystem::exists(asset.File))
        {
            Log("Attempted to load asset " + std::to_string(assetIdx) + " from invalid file '" + asset.File.string() + "'.", LogLevel::Error);
            asset.State = AssetState::Error;
            return;
        }

        // Load file to engine object asynchronously.
        asset.State = AssetState::Loading;
        g_Parallel.AddTask([&]()
        {
            // TODO: Parse to engine object.

            asset.State = AssetState::Loaded;
        });
    }

    void AssetManager::UnloadAsset(int assetIdx)
    {
        // Get asset.
        if (assetIdx < 0 || assetIdx >= _assets.size())
        {
            Log("Attempted to unload invalid asset " + std::to_string(assetIdx) + ".", LogLevel::Warning, LogMode::Debug);
            return;
        }
        auto& asset = _assets[assetIdx];

        // Check if already unloaded.
        if (asset.State == AssetState::Unloaded)
        {
            Log("Attempted to unload already unloaded asset " + std::to_string(assetIdx) + ".", LogLevel::Warning, LogMode::Debug);
            return;
        }

        // Unload.
        asset.State = AssetState::Unloaded;
        asset.Data.reset();
        Log("Unloaded asset " + std::to_string(assetIdx) + ".", LogLevel::Info, LogMode::Debug);
    }

    void AssetManager::UnloadAllAssets()
    {
        for (auto& asset : _assets)
        {
            asset.State = AssetState::Unloaded;
            asset.Data.reset();
        }

        Log("All assets unloaded.", LogLevel::Info, LogMode::Debug);
    }
}
