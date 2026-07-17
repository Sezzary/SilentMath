#include "Framework.h"
#include "Assets/Loaders/Ilm.h"

#include "Application.h"
#include "Assets/AssetStreamer.h"
#include "Renderer/Common/Resources/Layouts/Buffers.h"
#include "Renderer/Renderer.h"
#include "Utils/Stream.h"
#include "Utils/Utils.h"

using namespace Silent::Renderer;
using namespace Silent::Utils;

namespace Silent::Assets
{
    // @todo When extracting assets, some filenames need to be corrected for this to always point to a real texture.
    static std::string GetTextureName(const std::string& name)
    {
        auto path = stdfs::path(name);
        path.replace_extension(".TIM");
        return path.string();
    }

    std::shared_ptr<void> ParseIlm(const stdfs::path& filename)
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
                                         stdfs::relative(filename, fs.GetAssetsDirectory()).string()));
        }

        // Read header magic.
        int16 magic = stream.ReadInt16();
        if (magic != HEADER_MAGIC)
        {
            throw std::runtime_error(Fmt("Failed to parse invalid ILM `{}`.",
                                         stdfs::relative(filename, fs.GetAssetsDirectory()).string()));
        }

        uint8 isInitialized = stream.ReadUint8(); // Unused.

        stream.Skip(1);

        // Read name offset.
        uint32 nameOffset = stream.ReadUint32();
        if (nameOffset != HEADER_NAME_OFFSET)
        {
            throw std::runtime_error(Fmt("Attempted to parse ILM `{}` with incongruent name offset.",
                                         stdfs::relative(filename, fs.GetAssetsDirectory()).string()));
        }

        // Read header attributes.
        uint32 meshCount     = stream.ReadUint32();
        uint32 meshesOffset  = stream.ReadUint32();
        uint32 meshIdsOffset = stream.ReadUint32();
        auto   name          = stream.ReadNullString();

        // Set stream position to meshes.
        stream.SetPosition(meshesOffset);

        // Read meshes.
        auto meshes = std::vector<IlmMesh>{};
        meshes.reserve(meshCount);
        for (int i = 0; i < meshCount; i++)
        {
            // Create UV index lookup.
            auto uvLookup = std::unordered_map<Vector2i, int>{}; // Key = UV (Q0.8), value = UV index.

            // Read bone info.
            auto boneIdxStr = stream.ReadNullString(BONE_IDX_STR_SIZE);
            int  boneIdx    = std::stoi(boneIdxStr);
            auto boneName   = stream.ReadNullString(BONE_NAME_STR_SIZE);

            stream.Skip(1);

            // Create bone mesh.
            auto mesh = IlmMesh
            {
                .BoneIdx  = boneIdx,
                .BoneName = boneName
            };

            // Read base vertex indices.
            uint8 posBaseIdx    = stream.ReadUint8();
            uint8 normalBaseIdx = stream.ReadUint8();

            stream.Skip(1);

            // Read body offset.
            uint32 bodyOffset = stream.ReadUint32();

            // Set stream position to current mesh body.
            int returnPos = stream.GetPosition();
            stream.SetPosition(bodyOffset);

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
            uint32 nextBodyOffset = stream.ReadUint32(); // Unused.

            // Read primitives.
            stream.SetPosition(primsOffset);
            mesh.Native.Primitives.reserve(primCount);
            for (int j = 0; j < primCount; j++)
            {
                // Read UV 0.
                q0_8 uvX0 = stream.ReadUint8();
                q0_8 uvY0 = stream.ReadUint8();

                // Read palette index.
                int16 clutPos    = stream.ReadInt16();
                int   clutPosX   = (clutPos & 0x3F) * 0x10;
                int   clutPosZ   = (clutPos >> 6) & 0x1FF;
                int   paletteIdx = clutPosZ; // @todo Check if this is correct.

                // Read UV 1.
                q0_8 uvX1 = stream.ReadUint8();
                q0_8 uvY1 = stream.ReadUint8();

                // Read texture page.
                int16 tPage = stream.ReadInt16(); // Unused.

                // Read UV 2.
                q0_8 uvX2 = stream.ReadUint8();
                q0_8 uvY2 = stream.ReadUint8();

                // Read UV 3.
                q0_8 uvX3 = stream.ReadUint8();
                q0_8 uvY3 = stream.ReadUint8();

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

                bool isTri     = posIdx3 == UCHAR_MAX;
                int  vertCount = isTri ? TRI_VERTEX_COUNT : QUAD_VERTEX_COUNT;

                // Collect vertex indices.
                auto posIdxs = std::vector<int>
                {
                    posIdx0 - posBaseIdx,
                    posIdx1 - posBaseIdx,
                    posIdx2 - posBaseIdx
                };
                auto normalIdxs = std::vector<int>
                {
                    normalIdx0 - normalBaseIdx,
                    normalIdx1 - normalBaseIdx,
                    normalIdx2 - normalBaseIdx
                };
                auto uvIdxs = std::vector<int>
                {
                    GetLookupIdx(uvLookup, Vector2i(uvX0, uvY0)),
                    GetLookupIdx(uvLookup, Vector2i(uvX1, uvY1)),
                    GetLookupIdx(uvLookup, Vector2i(uvX2, uvY2))
                };
                if (!isTri)
                {
                    posIdxs.push_back(posIdx3 - posBaseIdx);
                    normalIdxs.push_back(normalIdx3 - normalBaseIdx);
                    uvIdxs.push_back(GetLookupIdx(uvLookup, Vector2i(uvX3, uvY3)));
                }

                // Create primitive.
                auto prim = IlmPrimitive{};

                // Collect vertices.
                prim.Vertices.reserve(vertCount);
                for (int k = 0; k < vertCount; k++)
                {
                    prim.Vertices.push_back(IlmVertex
                    {
                        .PositionIdx = posIdxs[k],
                        .NormalIdx   = normalIdxs[k],
                        .UvIdx       = uvIdxs[k],
                        .PaletteIdx  = paletteIdx
                    });
                }

                // Collect primitive.
                mesh.Native.Primitives.push_back(std::move(prim));
            }

            mesh.Native.Positions.reserve(posCount);

            // Read XY vertex position components.
            stream.SetPosition(posXyOffset);
            for (int j = 0; j < posCount; j++)
            {
                q8_7 x = stream.ReadInt16();
                q8_7 y = stream.ReadInt16();

                // Collect XZ position components.
                mesh.Native.Positions.push_back(Vector3i(x, y, 0));
            }

            // Read Z vertex position components.
            stream.SetPosition(posZOffset);
            for (int j = 0; j < posCount; j++)
            {
                q8_7 z = stream.ReadInt16();

                // Collect Z position component.
                mesh.Native.Positions[j].z = z;
            }

            // Read vertex normals.
            stream.SetPosition(normalsOffset);
            mesh.Native.Normals.reserve(normalCount);
            for (int j = 0; j < normalCount; j++)
            {
                q0_7  x     = stream.ReadInt8();
                q0_7  y     = stream.ReadInt8();
                q0_7  z     = stream.ReadInt8();
                uint8 count = stream.ReadUint8();

                // Collect normal.
                mesh.Native.Normals.push_back(Vector3i(x, y, z));
            }

            // Collect indexed UVs.
            mesh.Native.Uvs.resize(uvLookup.size());
            for (const auto& [keyUv, uvIdx] : uvLookup)
            {
                mesh.Native.Uvs[uvIdx] = keyUv;
            }

            // Collect mesh.
            meshes.push_back(std::move(mesh));

            // Reset stream position.
            stream.SetPosition(returnPos);
        }

        // Set stream position to mesh IDs.
        stream.SetPosition(meshIdsOffset);

        // Read mesh IDs.
        auto meshIds = std::vector<int>{};
        meshIds.reserve(meshCount);
        for (int i = 0; i < meshCount; i++)
        {
            uint8 meshId = stream.ReadUint8();

            // Collect mesh ID.
            meshIds.push_back(meshId);
        }

        // @todo Implement render buckets? Sort primitives by CLUT? Needs materials.
        // Linearize meshes.
        for (auto& mesh : meshes)
        {
            // Run through primitives.
            auto vertLookup = std::unordered_map<IlmVertex, int>{};
            for (const auto& prim : mesh.Native.Primitives)
            {
                // Collect vertex indices in lookup table.
                auto vertIdxs = std::vector<uint16>{};
                for (const auto& vert : prim.Vertices)
                {
                    uint16 newVertIdx = GetLookupIdx(vertLookup, vert);
                    vertIdxs.push_back(newVertIdx);
                }

                // Collect vertex indices.
                if (vertIdxs.size() == TRI_VERTEX_COUNT)
                {
                    mesh.Linear.Idxs.insert(mesh.Linear.Idxs.end(),
                    {
                        vertIdxs[0], vertIdxs[1], vertIdxs[2]
                    });
                }
                else if (vertIdxs.size() == QUAD_VERTEX_COUNT)
                {
                    mesh.Linear.Idxs.insert(mesh.Linear.Idxs.end(),
                    {
                        vertIdxs[0], vertIdxs[1], vertIdxs[2],
                        vertIdxs[1], vertIdxs[3], vertIdxs[2]
                    });
                }
            }

            // Collect indexed vertices.
            mesh.Linear.Vertices.resize(vertLookup.size());
            for (const auto& [keyVert, vertIdx] : vertLookup)
            {
                mesh.Linear.Vertices[vertIdx] = BufferVertex3d
                {
                    .Position   = mesh.Native.Positions[keyVert.PositionIdx].ToVector3() / 128.0f,
                    .Normal     = Vector3::Normalize(mesh.Native.Normals[keyVert.NormalIdx].ToVector3()),
                    .Uv         = mesh.Native.Uvs[keyVert.UvIdx].ToVector2() / 256.0f,
                    .PaletteIdx = keyVert.PaletteIdx
                };
            }
        }

        return std::make_shared<IlmAsset>(IlmAsset
        {
            .Name        = name,
            .TextureName = GetTextureName(name),
            .Meshes      = std::move(meshes),
            .MeshIds     = std::move(meshIds)
        });
    }

    void IlmQueueGpuUpload(const Asset& asset)
    {
        auto& renderer = g_App.GetRenderer();

        renderer.QueueMeshUpload(asset.Name);
    }

    void IlmQueueGpuRelease(const Asset& asset)
    {
        auto& renderer = g_App.GetRenderer();

        renderer.QueueMeshRelease(asset.Name);
    }
}
