#include "Framework.h"
#include "Assets/AssetStreamer.h"

#include "Application.h"
#include "Assets/Parsers/Anm.h"
#include "Assets/Parsers/Ilm.h"
#include "Assets/Parsers/Tim.h"
#include "Assets/Parsers/Tmd.h"
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
        { ".CMP", AssetType::Cmp },
        { "",     AssetType::Xa  } // @todo Should be given .XA extension when exporting from ROM.
    };

    static const auto PARSER_FUNCS = std::unordered_map<AssetType, std::function<std::shared_ptr<void>(const std::filesystem::path& file)>>
    {
        { AssetType::Anm, ParseAnm },
        { AssetType::Ilm, ParseIlm },
        { AssetType::Tim, ParseTim },
        { AssetType::Tmd, ParseTmd }
    };

    const std::string& AssetStreamer::GetName(int assetIdx) const
    {
        // Get asset.
        if (assetIdx < 0 || assetIdx >= _assets.size())
        {
            Debug::Log(Fmt("Attempted to get name of missing streamable asset {}.", assetIdx), Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return EMPTY_STRING;
        }
        const auto asset = _assets[assetIdx];

        return asset->Name;
    }

    std::vector<std::string> AssetStreamer::GetLoadedNames() const
    {
        // Run through registered assets.
        auto names = std::vector<std::string>{};
        for (const auto& asset : _assets)
        {
            if (asset->State == AssetState::Loaded)
            {
                names.push_back(asset->Name);
            }
        }

        return names;
    }

    const std::shared_ptr<Asset> AssetStreamer::GetAsset(int assetIdx)
    {
        // Get asset.
        if (assetIdx < 0 || assetIdx >= _assets.size())
        {
            Debug::Log(Fmt("Attempted to get missing streamable asset {}.", assetIdx), Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return nullptr;
        }
        const auto asset = _assets[assetIdx];

        // Load if not preloaded.
        if (asset->State != AssetState::Loaded)
        {
            Debug::Log(Fmt("Getting non-preloaded streamable asset `{}`. Loading in place.", GetName(assetIdx)),
                       Debug::LogLevel::Warning, Debug::LogMode::Debug);

            Load(assetIdx).wait();
        }

        // Check if loading failed.
        if (asset->State == AssetState::Error)
        {
            Debug::Log(Fmt("Failed to get streamable asset `{}`.", GetName(assetIdx)), Debug::LogLevel::Error, Debug::LogMode::Debug);
            return nullptr;
        }
        return asset;
    }

    const std::shared_ptr<Asset> AssetStreamer::GetAsset(const std::string& assetName)
    {
        // Check if asset exists.
        const int* assetIdx = Find(_idxs, assetName);
        if (assetIdx == nullptr)
        {
            Debug::Log(Fmt("Attempted to get missing streamable asset `{}`.", assetName), Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return nullptr;
        }

        // Get asset by index.
        return GetAsset(*assetIdx);
    }

    bool AssetStreamer::IsBusy() const
    {
        return _loadingCount > 0;
    }

    void AssetStreamer::Initialize(const std::filesystem::path& assetsPath)
    {
        // Collect files sorted alphabetically.
        auto files = std::vector<std::filesystem::path>{};
        for (auto& entry : std::filesystem::recursive_directory_iterator(assetsPath))
        {
            if (entry.is_regular_file())
            {
                files.push_back(entry.path().generic_string());
            }
        }
        Sort(files);

        // Register assets.
        _assets.reserve(files.size());
        for (int i = 0; i < files.size(); i++)
        {
            const auto& file = files[i];

            // Check if type is streamable.
            auto ext = ToUpper(file.extension().string());
            if (Find(ASSET_TYPES, ext) == nullptr)
            {
                //Debug::Log(Fmt("Attempted to register non-stremable asset for file `{}`.", file.string()), Debug::LogLevel::Warning, Debug::LogMode::Debug);
                continue;
            }

            // @heapalloc Create asset entry.
            _assets.emplace_back(std::make_shared<Asset>());

            // Define asset entry.
            auto asset   = _assets.back();
            asset->Name  = std::filesystem::relative(file, assetsPath).generic_string();
            asset->Type  = ASSET_TYPES.at(ext);
            asset->File  = file;
            asset->Size  = std::filesystem::file_size(file);
            asset->State = AssetState::Unloaded;
            asset->Data  = nullptr;

            // Add asset index and name to maps.
            _idxs[asset->Name] = i;
            _names[i]          = asset->Name;
        }

        // Create fallback ready future.
        _loadFutures[NO_VALUE] = GenerateReadyFuture();

        Debug::Log(Fmt("Registered {} streamable assets.", _assets.size()), Debug::LogLevel::Info, Debug::LogMode::Debug);
    }

    const std::future<void>& AssetStreamer::Load(int assetIdx)
    {
        auto& executor = g_App.GetExecutor();

        // Get asset.
        if (assetIdx < 0 || assetIdx >= _assets.size())
        {
            Debug::Log(Fmt("Attempted to load missing streamable asset {}.", assetIdx), Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return _loadFutures[NO_VALUE];
        }
        auto& asset = _assets[assetIdx];

        // Check if loading or loaded.
        if (asset->State == AssetState::Loading || asset->State == AssetState::Loaded)
        {
            return _loadFutures[assetIdx];
        }

        // Check if file is valid.
        if (!std::filesystem::exists(asset->File))
        {
            Debug::Log(Fmt("Attempted to load streamable asset `{}` from missing file `{}`.", asset->Name, asset->File.string()),
                       Debug::LogLevel::Error, Debug::LogMode::Debug);

            asset->State = AssetState::Error;
            return _loadFutures[assetIdx];
        }

        // Set loading state.
        asset->State = AssetState::Loading;
        _loadingCount++;

        // Load asynchronously.
        _loadFutures[assetIdx] = executor.AddTask([&]()
        {
            // Get parser function.
            const auto* parserFunc = Find(PARSER_FUNCS, asset->Type);
            if (parserFunc == nullptr)
            {
                Debug::Log(Fmt("Attempted to load streamable asset `{}` with no parser function for asset type {}.", asset->Name, (int)asset->Type),
                           Debug::LogLevel::Error);

                asset->State = AssetState::Unloaded;
                _loadingCount--;
                return;
            }

            // Parse asset data from file.
            try
            {
                asset->Data  = (*parserFunc)(asset->File);
                asset->State = AssetState::Loaded;

                Debug::Log(Fmt("Loaded streamable asset `{}`.", asset->Name), Debug::LogLevel::Info, Debug::LogMode::Debug);
            }
            catch (const std::exception& ex)
            {
                asset->Data  = nullptr;
                asset->State = AssetState::Error;

                Debug::Log(Fmt("Failed to parse file for streamable asset `{}`: {}", asset->Name, ex.what()), Debug::LogLevel::Error);
            }
            _loadingCount--;
        });

        return _loadFutures[assetIdx];
    }

    const std::future<void>& AssetStreamer::Load(const std::string& assetName)
    {
        // Check if asset exists.
        const int* assetIdx = Find(_idxs, assetName);
        if (assetIdx == nullptr)
        {
            Debug::Log(Fmt("Attempted to load unregistered streamable asset `{}`.", assetName), Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return _loadFutures[NO_VALUE];
        }

        // Load asset by index.
        return Load(*assetIdx);
    }

    void AssetStreamer::Unload(int assetIdx)
    {
        // Get asset.
        if (assetIdx < 0 || assetIdx >= _assets.size())
        {
            Debug::Log(Fmt("Attempted to unload missing streamable asset {}.", assetIdx), Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return;
        }
        auto& asset = _assets[assetIdx];

        // Check if already unloaded.
        if (asset->State == AssetState::Unloaded)
        {
            return;
        }

        // Unload.
        asset->State = AssetState::Unloaded;
        asset->Data  = nullptr;

        // Remove load future.
        _loadFutures.erase(assetIdx);

        Debug::Log(Fmt("Unloaded streamable asset `{}`.", GetName(assetIdx)), Debug::LogLevel::Info, Debug::LogMode::Debug);
    }

    void AssetStreamer::Unload(const std::string& assetName)
    {
        // Check if asset exists.
        const int* assetIdx = Find(_idxs, assetName);
        if (assetIdx == nullptr)
        {
            Debug::Log(Fmt("Attempted to unload unregistered streamable asset `{}`.", assetName), Debug::LogLevel::Warning, Debug::LogMode::Debug);
            return;
        }

        // Unload asset by index.
        Unload(*assetIdx);
    }

    void AssetStreamer::UnloadAll()
    {
        // Run through registered assets.
        for (auto& asset : _assets)
        {
            if (asset->State == AssetState::Unloaded)
            {
                continue;
            }

            // Unload.
            asset->State = AssetState::Unloaded;
            asset->Data  = nullptr;

            // Remove load future.
            _loadFutures.erase(_idxs[asset->Name]);
        }
    }
}
