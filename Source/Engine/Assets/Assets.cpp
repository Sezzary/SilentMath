#include "Framework.h"
#include "Engine/Assets/Assets.h"

#include "Utils/Parallel.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Assets
{
    struct ParsedDataBase
    {
        int Value = 0;
    };

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
        { ".CMP", AssetType::Cmp },
        { "",     AssetType::Xa }
    };

    static std::shared_ptr<ParsedDataBase> ParseTim(const std::filesystem::path& file)
    {
        return std::make_unique<ParsedDataBase>();
    }

    static std::shared_ptr<ParsedDataBase> ParseVab(const std::filesystem::path& file)
    {
        return std::make_unique<ParsedDataBase>();
    }

    static std::shared_ptr<ParsedDataBase> ParseDms(const std::filesystem::path& file)
    {
        return std::make_unique<ParsedDataBase>();
    }

    static std::shared_ptr<ParsedDataBase> ParseAnm(const std::filesystem::path& file)
    {
        return std::make_unique<ParsedDataBase>();
    }

    static std::shared_ptr<ParsedDataBase> ParsePlm(const std::filesystem::path& file)
    {
        return std::make_unique<ParsedDataBase>();
    }

    static std::shared_ptr<ParsedDataBase> ParseIpd(const std::filesystem::path& file)
    {
        return std::make_unique<ParsedDataBase>();
    }

    static std::shared_ptr<ParsedDataBase> ParseIlm(const std::filesystem::path& file)
    {
        return std::make_unique<ParsedDataBase>();
    }

    static std::shared_ptr<ParsedDataBase> ParseTmd(const std::filesystem::path& file)
    {
        return std::make_unique<ParsedDataBase>();
    }

    static std::shared_ptr<ParsedDataBase> ParseDat(const std::filesystem::path& file)
    {
        return std::make_unique<ParsedDataBase>();
    }

    static std::shared_ptr<ParsedDataBase> ParseKdt(const std::filesystem::path& file)
    {
        return std::make_unique<ParsedDataBase>();
    }

    static std::shared_ptr<ParsedDataBase> ParseCmp(const std::filesystem::path& file)
    {
        return std::make_unique<ParsedDataBase>();
    }

    static std::shared_ptr<ParsedDataBase> ParseXa(const std::filesystem::path& file)
    {
        return std::make_unique<ParsedDataBase>();
    }

    static const auto PARSER_FUNCS = std::unordered_map<AssetType, std::function<std::shared_ptr<ParsedDataBase>(const std::filesystem::path& file)>>
    {
        { AssetType::Tim, ParseTim },
        { AssetType::Vab, ParseVab },
        { AssetType::Dms, ParseDms },
        { AssetType::Anm, ParseAnm },
        { AssetType::Plm, ParsePlm },
        { AssetType::Ipd, ParseIpd },
        { AssetType::Ilm, ParseIlm },
        { AssetType::Tmd, ParseTmd },
        { AssetType::Dat, ParseDat },
        { AssetType::Kdt, ParseKdt },
        { AssetType::Cmp, ParseCmp },
        { AssetType::Xa , ParseXa }
    };

    const std::shared_ptr<Asset> AssetManager::GetAsset(int assetIdx) const
    {
        // Get asset.
        if (assetIdx < 0 || assetIdx >= _assets.size())
        {
            Log("Attempted to get invalid asset " + std::to_string(assetIdx) + ".", LogLevel::Warning, LogMode::Debug);
            return nullptr;
        }
        const auto asset = _assets[assetIdx];

        return asset;
    }

    bool AssetManager::IsBusy() const
    {
        return _busyCount > 0;
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
        for (int i = 0; i < files.size(); i++)
        {
            const auto& file = files[i];

            // Check if type is known.
            auto ext = ToUpper(file.extension().string());
            if (ASSET_TYPES.find(ext) == ASSET_TYPES.end())
            {
                Log("Unknown asset type for file '" + file.string() + "'.", LogLevel::Warning, LogMode::Debug);
                continue;
            }

            // Add asset.
            _assets.emplace_back(std::make_shared<Asset>());
            auto asset   = _assets.back();
            asset->Type  = ASSET_TYPES.at(ext);
            asset->File  = file;
            asset->Size  = std::filesystem::file_size(file);
            asset->State = AssetState::Unloaded;
            asset->Data  = nullptr;
        }

        Log("Registered " + std::to_string(_assets.size()) + " assets from '" + assetsPath.string() + "'.", LogLevel::Info, LogMode::Debug);
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
        if (asset->State == AssetState::Loading || asset->State == AssetState::Loaded)
        {
            Log("Attempted to load already loading/loaded asset " + std::to_string(assetIdx) + ".", LogLevel::Warning, LogMode::Debug);
            return;
        }

        // Check if file is valid.
        if (!std::filesystem::exists(asset->File))
        {
            Log("Attempted to load asset " + std::to_string(assetIdx) + " from invalid file '" + asset->File.string() + "'.", LogLevel::Error);

            asset->State = AssetState::Error;
            return;
        }

        // Set busy state.
        _busyCount++;
        asset->State = AssetState::Loading;

        // Load asynchronously.
        g_Parallel.AddTask([&]()
        {
            // Get parser function.
            auto parserIt = PARSER_FUNCS.find(asset->Type);
            if (parserIt == PARSER_FUNCS.end())
            {
                Log("Attempted to load asset " + std::to_string(assetIdx) + ". No parser function for asset type " + std::to_string((int)asset->Type) + ".",
                    LogLevel::Error, LogMode::Debug);

                _busyCount--;
                asset->State = AssetState::Error;
                return;
            }
            const auto& parserFunc = parserIt->second;

            // Parse file to engine object.
            try
            {
                asset->Data  = parserFunc(asset->File);
                asset->State = AssetState::Loaded;
            }
            catch (const std::exception& ex)
            {
                Log("Failed to parse data for file " + std::to_string(assetIdx) + ": " + ex.what(), LogLevel::Error, LogMode::Debug);

                asset->State = AssetState::Error;
            }
            _busyCount--;
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
        if (asset->State == AssetState::Unloaded)
        {
            Log("Attempted to unload already unloaded asset " + std::to_string(assetIdx) + ".", LogLevel::Warning, LogMode::Debug);
            return;
        }

        // Unload.
        asset->State = AssetState::Unloaded;
        asset->Data.reset();

        Log("Unloaded asset " + std::to_string(assetIdx) + ".", LogLevel::Info, LogMode::Debug);
    }

    void AssetManager::UnloadAllAssets()
    {
        for (auto& asset : _assets)
        {
            asset->State = AssetState::Unloaded;
            asset->Data.reset();
        }

        Log("All assets unloaded.", LogLevel::Info, LogMode::Debug);
    }
}
