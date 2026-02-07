#include "Framework.h"
#include "Assets/Parsers/Ilm.h"

#include "Application.h"
#include "Utils/Stream.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Assets
{
    std::shared_ptr<void> ParseIlm(const std::filesystem::path& filename)
    {
        constexpr int16  HEADER_MAGIC       = 0x630;
        constexpr uint32 HEADER_NAME_OFFSET = 0x14;
        constexpr int    BONE_IDX_STR_SIZE  = 2;
        constexpr int    BONE_NAME_STR_SIZE = 6;

        const auto& fs = g_App.GetFilesystem();

        // Read file.
        auto stream = Stream(filename, true, false);
        if (!stream.IsOpen())
        {
            throw std::runtime_error(Fmt("Failed to open ILM `{}`.",
                                         std::filesystem::relative(fs.GetAssetsDirectory(), filename).string()));
        }

        // Read header magic.
        int16 magic = stream.ReadInt16();
        if (magic != HEADER_MAGIC)
        {
            throw std::runtime_error(Fmt("Failed to parse invalid ILM `{}`.",
                                         std::filesystem::relative(fs.GetAssetsDirectory(), filename).string()));
        }

        uint8 isInitialized = stream.ReadUint8();
        stream.Skip(1);

        // Read name offset.
        uint32 nameOffset = stream.ReadUint32();
        if (nameOffset != HEADER_NAME_OFFSET)
        {
            throw std::runtime_error(Fmt("Attempted to parse ILM `{}` with incongruent name offset.",
                                         std::filesystem::relative(fs.GetAssetsDirectory(), filename).string()));
        }

        // Read header attributes.
        uint32 meshCount    = stream.ReadUint32();
        uint32 meshesOffset = stream.ReadUint32();
        uint32 idsOffset    = stream.ReadUint32(); // @todo IDs of what?
        auto   name         = stream.ReadNullString();

        // Create asset.
        auto asset = IlmAsset{};

        // Read meshes.
        stream.SetPosition(meshesOffset);
        asset.Meshes.reserve(meshCount);
        for (int i = 0; i < meshCount; i++)
        {
            auto mesh = IlmMesh{};

            // Create UV index lookup.
            auto uvLookup = std::unordered_map<Vector2, int>{}; // Key = UV, value = UV index.

            // Read bone info.
            auto boneIdxStr = stream.ReadNullString(BONE_IDX_STR_SIZE);
            int  boneIdx    = std::stoi(boneIdxStr);
            auto boneName   = stream.ReadNullString(BONE_NAME_STR_SIZE);
            stream.Skip(1);

            // Read base vertex indices.
            uint8 posBaseIdx    = stream.ReadUint8();
            uint8 normalBaseIdx = stream.ReadUint8();
            stream.Skip(1);

            // Read mesh offset.
            uint32 meshOffset = stream.ReadUint32();

            // Set stream position to current mesh.
            int returnPos = stream.GetPosition();
            stream.SetPosition(meshOffset);

            // Read mesh component counts.
            uint8 primCount   = stream.ReadUint8();
            uint8 posCount    = stream.ReadUint8();
            uint8 normalCount = stream.ReadUint8();
            stream.Skip(1);

            // Read mesh component offsets.
            uint32 primsOffset    = stream.ReadUint32();
            uint32 posXyOffset    = stream.ReadUint32();
            uint32 posZOffset     = stream.ReadUint32();
            uint32 normalsOffset  = stream.ReadUint32();
            uint32 nextMeshOffset = stream.ReadUint32(); // `meshOffset` of next mesh. Unused.

            // Read primitives.
            stream.SetPosition(primsOffset);
            mesh.Primitives.reserve(primCount);
            for (int j = 0; j < primCount; j++)
            {
                // Read UV 0.
                uint8 uvX0 = stream.ReadInt8();
                uint8 uvY0 = stream.ReadInt8();

                // Read CLUT index. @todo
                int16 clutIdx = stream.ReadInt16(); // @todo Packed XY value.

                // Read UV 1.
                uint8 uvX1 = stream.ReadInt8();
                uint8 uvY1 = stream.ReadInt8();

                // Read tpage info. @todo
                int16 tpageInfo = stream.ReadInt16();

                // Read UV 2.
                uint8 uvX2 = stream.ReadInt8();
                uint8 uvY2 = stream.ReadInt8();

                // Read UV 3.
                uint8 uvX3 = stream.ReadInt8();
                uint8 uvY3 = stream.ReadInt8();

                // Read vertex positions.
                uint8 posIdx0 = stream.ReadUint8();
                uint8 posIdx1 = stream.ReadUint8();
                uint8 posIdx2 = stream.ReadUint8();
                uint8 posIdx3 = stream.ReadUint8();

                // Read vertex normals.
                uint8 normalIdx0 = stream.ReadUint8();
                uint8 normalIdx1 = stream.ReadUint8();
                uint8 normalIdx2 = stream.ReadUint8();
                uint8 normalIdx3 = stream.ReadUint8();

                bool isTri     = posIdx3 == UINT_MAX;
                int  vertCount = isTri ? TRI_VERTEX_COUNT : QUAD_VERTEX_COUNT;

                // Collect vertex indices.
                auto posIdxs = std::vector<int>
                {
                    posBaseIdx + posIdx0,
                    posBaseIdx + posIdx1,
                    posBaseIdx + posIdx2
                };
                auto normalIdxs = std::vector<int>
                {
                    normalBaseIdx + normalIdx0,
                    normalBaseIdx + normalIdx1,
                    normalBaseIdx + normalIdx2
                };
                auto uvIdxs = std::vector<int>
                {
                    GetLookupIdx(uvLookup, Vector2(uvX0, uvY0) / (float)UCHAR_MAX),
                    GetLookupIdx(uvLookup, Vector2(uvX1, uvY1) / (float)UCHAR_MAX),
                    GetLookupIdx(uvLookup, Vector2(uvX2, uvY2) / (float)UCHAR_MAX)
                };
                if (!isTri)
                {
                    posIdxs.push_back(posBaseIdx + posIdx3);
                    normalIdxs.push_back(normalBaseIdx + normalIdx3);
                    uvIdxs.push_back(GetLookupIdx(uvLookup, Vector2(uvX3, uvY3) / (float)UCHAR_MAX));
                }

                // Create primitive.
                auto prim = IlmPrimitive{};
                prim.Vertices.reserve(vertCount);
                for (int k = 0; k < vertCount; k++)
                {
                    prim.Vertices.push_back(IlmVertex
                    {
                        .PositionIdx = posIdxs[k],
                        .NormalIdx   = normalIdxs[k],
                        .UvIdx       = uvIdxs[k]
                    });
                }

                // Collect primitive.
                mesh.Primitives.push_back(std::move(prim));
            }

            mesh.Positions.reserve(posCount);

            // Read XY vertex position components.
            stream.SetPosition(posXyOffset);
            for (int j = 0; j < posCount; j++)
            {
                int16 x = stream.ReadInt16();
                int16 y = stream.ReadInt16();

                // Collect XZ position components.
                mesh.Positions.push_back(Vector3(x, y, 0.0f));
            }

            // Read Z vertex components.
            stream.SetPosition(posZOffset);
            for (int j = 0; j < posCount; j++)
            {
                int16 z = stream.ReadInt16();

                // Collect Z position component.
                mesh.Positions[j].z = z;
            }

            // Read vertex normals.
            stream.SetPosition(normalsOffset);
            mesh.Normals.reserve(normalCount);
            for (int j = 0; j < normalCount; j++)
            {
                int8  x     = stream.ReadInt8();
                int8  y     = stream.ReadInt8();
                int8  z     = stream.ReadInt8();
                uint8 count = stream.ReadUint8();

                // Collect normal.
                mesh.Normals.push_back(Vector3::Normalize(Vector3(x, y, z)));
            }

            // Collect indexed UVs.
            mesh.Uvs.resize(uvLookup.size());
            for (const auto& [keyUv, uvIdx] : uvLookup)
            {
                mesh.Uvs[uvIdx] = keyUv;
            }

            // Collect mesh.
            asset.Meshes.push_back(std::move(mesh));

            // Reset stream position.
            stream.SetPosition(returnPos);
        }

        // Read IDs. @todo What's this?
        stream.SetPosition(idsOffset);
        asset.Ids.reserve(meshCount);
        for (int i = 0; i < meshCount; i++)
        {
            uint8 id = stream.ReadUint8();

            // Collect ID.
            asset.Ids.push_back(id);
        }

        return std::make_shared<IlmAsset>(std::move(asset));
    }
}
