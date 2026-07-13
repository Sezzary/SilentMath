#include "Framework.h"
#include "Assets/Loaders/Ilm.h"

#include "Application.h"
#include "Renderer/Common/Resources/Buffers.h"
#include "Utils/Stream.h"
#include "Utils/Utils.h"

using namespace Silent::Renderer;
using namespace Silent::Utils;

namespace Silent::Assets
{
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

        // Create asset.
        auto asset = IlmAsset
        {
            .Name = name
        };

        // Read meshes.
        stream.SetPosition(meshesOffset);
        asset.Meshes.reserve(meshCount);
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
            uint32 nextBodyOffset = stream.ReadUint32(); // `bodyOffset` of next mesh. Unused.

            // Read primitives.
            stream.SetPosition(primsOffset);
            mesh.Native.Primitives.reserve(primCount);
            for (int j = 0; j < primCount; j++)
            {
                // Read UV 0.
                uint8 uvX0 = stream.ReadUint8(); // Q0.8
                uint8 uvY0 = stream.ReadUint8(); // Q0.8

                // Read CLUT indices.
                int16 clutPos  = stream.ReadInt16(); // Unused.
                int   clutPosX = (clutPos & 0x3F) * 0x10;
                int   clutPosZ = (clutPos >> 6) & 0x1FF;
                int   clutIdx  = clutPosZ; // @todo Check. Layout in VRAM appears to be simple.

                // Read UV 1.
                uint8 uvX1 = stream.ReadUint8(); // Q0.8
                uint8 uvY1 = stream.ReadUint8(); // Q0.8

                // Read texture page.
                int16 tPage = stream.ReadInt16();

                // Read UV 2.
                uint8 uvX2 = stream.ReadUint8(); // Q0.8
                uint8 uvY2 = stream.ReadUint8(); // Q0.8

                // Read UV 3.
                uint8 uvX3 = stream.ReadUint8(); // Q0.8
                uint8 uvY3 = stream.ReadUint8(); // Q0.8

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
                auto prim = IlmPrimitive
                {
                    .TPage = tPage
                };

                // Collect vertices.
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
                mesh.Native.Primitives.push_back(std::move(prim));
            }

            mesh.Native.Positions.reserve(posCount);

            // Read XY vertex position components.
            stream.SetPosition(posXyOffset);
            for (int j = 0; j < posCount; j++)
            {
                int16 x = stream.ReadInt16(); // Q8.7
                int16 y = stream.ReadInt16(); // Q8.7

                // Collect XZ position components.
                mesh.Native.Positions.push_back(Vector3i(x, y, 0));
            }

            // Read Z vertex position components.
            stream.SetPosition(posZOffset);
            for (int j = 0; j < posCount; j++)
            {
                int16 z = stream.ReadInt16(); // Q8.7

                // Collect Z position component.
                mesh.Native.Positions[j].z = z;
            }

            // Read vertex normals.
            stream.SetPosition(normalsOffset);
            mesh.Native.Normals.reserve(normalCount);
            for (int j = 0; j < normalCount; j++)
            {
                int8  x     = stream.ReadInt8(); // Q0.7
                int8  y     = stream.ReadInt8(); // Q0.7
                int8  z     = stream.ReadInt8(); // Q0.7
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
            asset.Meshes.push_back(std::move(mesh));

            // Reset stream position.
            stream.SetPosition(returnPos);
        }

        // Read mesh IDs.
        stream.SetPosition(meshIdsOffset);
        asset.MeshIds.reserve(meshCount);
        for (int i = 0; i < meshCount; i++)
        {
            uint8 meshId = stream.ReadUint8();

            // Collect mesh ID.
            asset.MeshIds.push_back(meshId);
        }

        // @todo Implement render buckets? Sort primitives by CLUT? Needs materials.
        // Convert to linear meshes.
        for (auto& mesh : asset.Meshes)
        {
            // Run through primitives.
            auto vertLookup = std::unordered_map<IlmVertex, int>{};
            for (const auto& prim : mesh.Native.Primitives)
            {
                // Collect primitive vertex indices.
                auto primIdxs = std::vector<uint16>{};
                for (const auto& vert : prim.Vertices)
                {
                    uint16 newIdx = GetLookupIdx(vertLookup, vert);
                    primIdxs.push_back(newIdx);
                }

                // Collect vertex indices.
                if (primIdxs.size() == TRI_IDX_COUNT)
                {
                    mesh.Linear.Idxs.insert(mesh.Linear.Idxs.end(),
                    {
                        primIdxs[0], primIdxs[1], primIdxs[2]
                    });
                }
                else if (primIdxs.size() == QUAD_IDX_COUNT)
                {
                    mesh.Linear.Idxs.insert(mesh.Linear.Idxs.end(),
                    {
                        primIdxs[0], primIdxs[1], primIdxs[2], 
                        primIdxs[0], primIdxs[2], primIdxs[3]
                    });
                }
            }

            // Collect indexed vertices.
            mesh.Linear.Vertices.resize(vertLookup.size());
            for (const auto& [keyVert, vertIdx] : vertLookup)
            {
                mesh.Linear.Vertices[vertIdx] = BufferVertex3d
                {
                    .Position = mesh.Native.Positions[keyVert.PositionIdx].ToVector3() / 128.0f,
                    .Normal   = Vector3::Normalize(mesh.Native.Normals[keyVert.NormalIdx].ToVector3()),
                    .Uv       = mesh.Native.Uvs[keyVert.UvIdx].ToVector2() / 256.0f,
                    .Col      = Color::White
                };
            }
        }

        // @debug 2 green test triangles.
        asset.Meshes.push_back(
            IlmMesh
            {
                .BoneIdx    = 0,
                .BoneName   = "TEST",
                .Linear = IlmLinearMesh
                {
                    .Vertices =
                    {
                        BufferVertex3d
                        {
                            .Position = { 0.1328125, -0.1796875, -0.0234375 },
                            .Normal   = Vector3::One,
                            .Uv       = Vector2::Zero,
                            .Col      = Color::Green
                        },
                        BufferVertex3d
                        {
                            .Position = { 0.0625, -0.0546875, -0.1015625 },
                            .Normal   = Vector3::One,
                            .Uv       = Vector2::Zero,
                            .Col      = Color::Green
                        },
                        BufferVertex3d
                        {
                            .Position = { 0.0703125, -0.203125, -0.125 },
                            .Normal   = Vector3::One,
                            .Uv       = Vector2::Zero,
                            .Col      = Color::Green
                        },
                        BufferVertex3d
                        {
                            .Position = { -0.0859375, -0.0546875, -0.1015625 },
                            .Normal   = Vector3::One,
                            .Uv       = Vector2::Zero,
                            .Col      = Color::Green
                        }
                    },
                    .Idxs = { 0, 1, 2, 1, 3, 2 }
                }
            });

        return std::make_shared<IlmAsset>(std::move(asset));
    }
}
