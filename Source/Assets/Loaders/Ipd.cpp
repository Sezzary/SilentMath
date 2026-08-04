#include "Framework.h"
#include "Assets/Loaders/Ipd.h"

#include "Application.h"
#include "Assets/Loaders/Utils/Lm.h"
#include "Utils/Stream.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Assets
{
    /** @brief Normalizes the UVs of meshes according to an IPD model's associated texture resolution.
     *
     * @param name Model name.
     * @param meshes Model meshes with UVs to normalize.
     */
    static void NormalizeUvs(const std::string& name, std::vector<LmMesh>& meshes)
    {
        constexpr int RES_STEP = 128;

        static const auto IPD_TEX_RES_MAP = std::unordered_map<std::string, Vector2i>
        {
            // @todo Account for some exceptions. Most textures are either 128x256 or 256x256, but some are thin strips
            // or the UVs don't span far enough.
        };

        auto texRes = Vector2i::Zero;

        // Get texture resolution.
        const auto* obtuseTexRes = Find(IPD_TEX_RES_MAP, name);
        if (obtuseTexRes != nullptr)
        {
            texRes = *obtuseTexRes;
        }
        else
        {
            // Derive resolution from pixel UV layout.
            for (const auto& mesh : meshes)
            {
                for (const auto& vert : mesh.Linear.Vertices)
                {
                    texRes = Vector2i(std::max(texRes.x, (int)CeilToStep(vert.Uv.x, RES_STEP)),
                                      std::max(texRes.y, (int)CeilToStep(vert.Uv.y, RES_STEP)));
                }
            }
        }

        // Normalize UVs.
        for (auto& mesh : meshes)
        {
            for (auto& vert : mesh.Linear.Vertices)
            {
                vert.Uv /= texRes.ToVector2();
            }
        }
    }

    std::shared_ptr<void> ParseIpd(const stdfs::path& filename)
    {
        constexpr uint8 MAGIC = 0x14;

        const auto& fs = g_App.GetFilesystem();

        auto asset = IpdAsset{};

        // Read file.
        auto stream = Stream(filename, true, false);
        if (!stream.IsOpen())
        {
            throw std::runtime_error(Fmt("Failed to open IPD `{}`.",
                                         stdfs::relative(filename, fs.GetAssetsDirectory()).string()));
        }

        // Read header magic.
        uint8 magic = stream.ReadUint8();
        if (magic != MAGIC)
        {
            throw std::runtime_error(Fmt("Failed to parse invalid IPD `{}`.",
                                         stdfs::relative(filename, fs.GetAssetsDirectory()).string()));
        }

        // Read header.
        uint8  isInitialized       = stream.ReadUint8();
        int8   levelGridX          = stream.ReadInt8();
        int8   levelGridZ          = stream.ReadInt8();
        uint32 lmOffset            = stream.ReadUint32();
        uint8  modelCount          = stream.ReadUint8();
        uint8  modelBufferCount    = stream.ReadUint8();
        uint8  modelOrderCount     = stream.ReadUint8();
        stream.Skip(9); // Unknown.
        uint32 modelInfosOffset    = stream.ReadUint32();
        uint32 modelBuffersOffset  = stream.ReadUint32();
        uint8  textureCount        = stream.ReadUint8();
        stream.Skip(51); // Unknown. @todo
        uint32 modelOrderListOffset = stream.ReadUint32();

        // @todo Read collision.
        stream.Skip(52);

        // Set stream position to LM chunk.
        stream.SetPosition(lmOffset);

        // Read LM chunk.
        auto meshes  = std::vector<LmMesh>{};
        auto meshIds = std::vector<int>{};
        auto name    = ParseLmChunk(filename, stream, meshes, meshIds);

        // Normalize IPD UVs.
        NormalizeUvs(name, meshes);

        // Set LM chunk.
        asset.Lm = LmChunk
        {
            .Name        = name,
            //.TextureName = GetTextureAssetName(name),// @todo
            .Meshes      = std::move(meshes),
            .MeshIds     = std::move(meshIds)
        };

        // Set stream position to model infos.
        stream.SetPosition(modelInfosOffset);

        // Read model infos.
        auto modelInfos = std::vector<IpdModelInfo>{};
        modelInfos.reserve(modelCount);
        for (int i = 0; i < modelCount; i++)
        {
            auto modelInfo        = IpdModelInfo{};
            modelInfo.IsGlobalPlm = stream.ReadUint8();

            stream.Skip(3);

            modelInfo.Name              = stream.ReadCString(8);
            modelInfo.ModelHeaderOffset = stream.ReadUint32();
            modelInfos.push_back(std::move(modelInfo));
        }

        // Set stream position to model buffers.
        stream.SetPosition(modelBuffersOffset);

        // @todo
        // Read model buffers.
        for (int i = 0; i < modelBufferCount; i++)
        {
            uint8 modelInstCount = stream.ReadUint8();
            uint8 field_1        = stream.ReadUint8();

            stream.Skip(1);

            int16  minX                   = stream.ReadInt16();
            int16  maxX                   = stream.ReadInt16();
            int16  minZ                   = stream.ReadInt16();
            int16  maxZ                   = stream.ReadInt16();
            uint32 modelInstsOffset       = stream.ReadUint32();
            uint32 field_10Offset         = stream.ReadUint32();
            uint32 subcellPositionsOffset = stream.ReadUint32();
        }

        return std::make_shared<IpdAsset>(std::move(asset));
    }

    void QueueIpdGpuUpload(const Asset& asset)
    {
        auto& renderer = g_App.GetRenderer();

        renderer.QueueMeshUpload(asset.Name);
    }

    void QueueIpdGpuRelease(const Asset& asset)
    {
        auto& renderer = g_App.GetRenderer();

        renderer.QueueMeshRelease(asset.Name);
    }
}
