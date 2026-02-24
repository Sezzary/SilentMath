#include "Framework.h"
#include "Assets/Loaders/Tmd.h"

#include "Application.h"
#include "Renderer/Common/Enums.h"
#include "Renderer/Common/Resources/Buffers.h"
#include "Utils/Stream.h"
#include "Utils/Utils.h"

using namespace Silent::Renderer;
using namespace Silent::Utils;

namespace Silent::Assets
{
    /** @brief TMD flags. */
    enum class TmdFlags
    {
        Fixp = 1 << 0 /** 0: Relative offset from mesh data block start, 1: Absolute offset from file start. */
    };

    /** @brief TMD primitive flags. */
    enum class TmdPrimitiveFlags
    {
        Lit         = 1 << 0, /** 0: Use light, 1: Use raw color. */
        DoubleSided = 1 << 1, /** 0: Single-sided, 1: Double-sided. */
        Graded      = 1 << 2  /** 0: Fixed color, 1: Gradient. */
    };

    /** @brief TMD packed primitive modes. */
    enum class TmdPrimitiveModes
    {
        Brightness   = 1 << 0,
        Transparency = 1 << 1,                         /** 0: Opaque, 1: Semi-transparent. */
        Textured     = 1 << 2,                         /** 0: Untextured, 1: Textured. */
        Quad         = 1 << 3,                         /** 0: Triangle, 1: Quad. */
        Gouraud      = 1 << 4,                         /** 0: Flat, 1: Gouraud. */
        Primitive    = (1 << 5) | (1 << 6) | (1 << 7)  /** `TmdPrimitiveType` */
    };

    /** @brief TMD packed texture attributes. */
    enum class TmdTextureAttribs
    {
        TPage        = (1 << 0) | (1 << 2) | (1 << 3) | (1 << 4), /** PSX texture page. */
        BlendMode    = (1 << 5) | (1 << 6),                       /** `TmdBlendMode` */
        TextureDepth = (1 << 7) | (1 << 8)                        /** Unused. */
    };

    /** TMD primitive types. */
    enum class TmdPrimitiveType
    {
        Polygon = 1,
        Line    = 2,
        Sprite  = 3
    };

    /** @brief TMD textured primitive blend modes. */
    enum class TmdBlendMode
    {
        AlphaHalf  = 0,
        Add        = 1,
        Subtract   = 2,
        AddQuarter = 3
    };

    /** @brief TMD header layout. */
    struct TmdHeaderLayout
    {
        uint32 Version   = 0; /** Unused. */
        uint32 Flags     = 0;
        uint32 MeshCount = 0;
    };

    /** @brief TMD mesh description layout. */
    struct TmdMeshDescLayout
    {
        uint32 PositionOffset  = 0;
        uint32 PositionCount   = 0;
        uint32 NormalOffset    = 0;
        uint32 NormalCount     = 0;
        uint32 PrimitiveOffset = 0;
        uint32 PrimitiveCount  = 0;
        uint32 Scale           = 0; /** Unused. */
    };

    /** @brief Converts a packed TMD RGB vertex color to a renderer color.
     *
     * @param color Packed TMD RGB color.
     * @param colorAlpha Color alpha.
     * @return Converted renderer color.
     */
    static Color ConvertTmdVertexColor(uint32 color, float colorAlpha)
    {
        return Color::From8Bit(color & UCHAR_MAX,
                               (color >> 8) & UCHAR_MAX,
                               (color >> 16) & UCHAR_MAX,
                               Q8_COLOR_FROM(colorAlpha));
    }

    /** @brief Gets the blend mode and color alpha of a TMD primitive.
     *
     * @param blendMode Output blend mode.
     * @param colorAlpha Output color alpha.
     * @param hasTransparency Primitive is transparent.
     * @param tmdBlendMode TMD blend mode.
     */
    static void GetTmdBlendModeAndColorAlpha(BlendMode& blendMode, float& colorAlpha, bool hasTransparency, TmdBlendMode tmdBlendMode)
    {
        if (hasTransparency)
        {
            switch (tmdBlendMode)
            {
                case TmdBlendMode::AlphaHalf:
                {
                    blendMode  = BlendMode::Alpha;
                    colorAlpha = 0.5f;
                    break;
                }
                case TmdBlendMode::Add:
                {
                    blendMode  = BlendMode::Add;
                    colorAlpha = 1.0f;
                    break;
                }
                case TmdBlendMode::Subtract:
                {
                    blendMode  = BlendMode::Subtract;
                    colorAlpha = 1.0f;
                    break;
                }
                case TmdBlendMode::AddQuarter:
                {
                    blendMode  = BlendMode::Add;
                    colorAlpha = 0.25f;
                    break;
                }
            }
        }
        else
        {
            blendMode  = BlendMode::Opaque;
            colorAlpha = 1.0f;
        }
    }

    std::shared_ptr<void> ParseTmd(const std::filesystem::path& filename)
    {
        const auto& fs = g_App.GetFilesystem();

        // Read file.
        auto stream = Stream(filename, true, false);
        if (!stream.IsOpen())
        {
            throw std::runtime_error(Fmt("Failed to open TMD `{}`.",
                                         std::filesystem::relative(filename, fs.GetAssetsDirectory()).string()));
        }

        // Read header.
        auto header = TmdHeaderLayout
        {
            .Version   = stream.ReadUint32(),
            .Flags     = stream.ReadUint32(),
            .MeshCount = stream.ReadUint32()
        };

        // Compute base data address.
        int baseAddr = sizeof(TmdHeaderLayout) + (header.MeshCount * sizeof(TmdMeshDescLayout));

        // Read mesh descriptions.
        auto meshDescs = std::vector<TmdMeshDescLayout>{};
        meshDescs.reserve(header.MeshCount);
        for (int i = 0; i < header.MeshCount; i++)
        {
            auto meshDesc = TmdMeshDescLayout{};

            // Read vertex data.
            meshDesc.PositionOffset = stream.ReadUint32();
            meshDesc.PositionCount  = stream.ReadUint32();

            // Read normal data.
            meshDesc.NormalOffset = stream.ReadUint32();
            meshDesc.NormalCount  = stream.ReadUint32();

            // Read primitive data.
            meshDesc.PrimitiveOffset = stream.ReadUint32();
            meshDesc.PrimitiveCount  = stream.ReadUint32();

            // Read scale.
            meshDesc.Scale = stream.ReadUint32();

            // Adjust offsets.
            if (header.Flags & (int)TmdFlags::Fixp)
            {
                meshDesc.PositionOffset  -= baseAddr;
                meshDesc.NormalOffset    -= baseAddr;
                meshDesc.PrimitiveOffset -= baseAddr;
            }

            // Collect mesh description.
            meshDescs.push_back(std::move(meshDesc));
        }

        // Create asset.
        auto asset = TmdAsset
        {
            .Meshes = std::vector<TmdMesh>(header.MeshCount)
        };

        // Read meshes.
        for (int i = 0; i < header.MeshCount; i++)
        {
            const auto& meshDesc = meshDescs[i];

            auto mesh = TmdMesh{};

            // @todo Texture name?

            // Create UV and color index lookups.
            auto uvLookup    = std::unordered_map<Vector2, int>{}; // Key = UV, value = UV index.
            auto colorLookup = std::unordered_map<Color,   int>{}; // Key = color, value = color index.

            // Read vertex positions.
            stream.SetPosition(baseAddr + meshDesc.PositionOffset);
            mesh.Positions.reserve(meshDesc.PositionCount);
            for (int j = 0; j < meshDesc.PositionCount; j++)
            {
                // Read components.
                int16 x = stream.ReadInt16();
                int16 y = stream.ReadInt16();
                int16 z = stream.ReadInt16();
                stream.Skip(2);

                // Collect position.
                mesh.Positions.push_back(Vector3(x, y, z));
            }

            // Read vertex normals.
            stream.SetPosition(baseAddr + meshDesc.NormalOffset);
            mesh.Normals.reserve(meshDesc.NormalCount);
            for (int j = 0; j < meshDesc.NormalCount; j++)
            {
                // Read components.
                int16 x = stream.ReadInt16();
                int16 y = stream.ReadInt16();
                int16 z = stream.ReadInt16();
                stream.Skip(2);

                // Collect normal.
                auto normal = Vector3::Normalize(Vector3(x, y, z));
                mesh.Normals.push_back(normal);
            }

            // Read primitives.
            stream.SetPosition(baseAddr + meshDesc.PrimitiveOffset);
            mesh.Primitives.reserve(meshDesc.PrimitiveCount);
            for (int j = 0; j < meshDesc.PrimitiveCount; j++)
            {
                // Read attributes.
                int8 olen  = stream.ReadInt8(); // Unused.
                int8 ilen  = stream.ReadInt8(); // Packet size in words.
                int8 flags = stream.ReadInt8(); // `TmdPrimitiveFlags`
                int8 mode  = stream.ReadInt8(); // `TmdPrimitiveModes`

                bool isGouraud = mode & (int)TmdPrimitiveModes::Gouraud;

                // Compute next primitive position.
                int nextPrimPos = stream.GetPosition() + (ilen * sizeof(int32));

                // Read primitive.
                auto primType = (TmdPrimitiveType)((mode & (int)TmdPrimitiveModes::Primitive) >> 5);
                switch (primType)
                {
                    case TmdPrimitiveType::Polygon:
                    {
                        // Read attributes.
                        int  vertCount = (mode & (int)TmdPrimitiveModes::Quad) ? QUAD_VERTEX_COUNT : TRI_VERTEX_COUNT;
                        auto uvs       = std::vector<Vector2>(vertCount, Vector2::Zero);
                        auto colors    = std::vector<Color>(vertCount, Color::White);
                        auto blendMode = BlendMode::Opaque;
                        int  tpage     = 0;
                        bool isTri     = vertCount == TRI_VERTEX_COUNT;

                        // Read textured polygon.
                        if (mode & (int)TmdPrimitiveModes::Textured)
                        {
                            // Read UVs, CBA, and TSB.
                            uint16 cba = 0; // Unused.
                            uint16 tsb = 0;
                            for (int i = 0; i < vertCount; i++)
                            {
                                // Read UV.
                                uint8 u = stream.ReadUint8();
                                uint8 v = stream.ReadUint8();

                                // Set normalized UV.
                                uvs[i] = Vector2(u, v) / (float)UCHAR_MAX;

                                // Read CBA.
                                if (i == 0)
                                {
                                    cba = stream.ReadUint16();
                                }
                                // Read TSB.
                                else if (i == 1)
                                {
                                    tsb = stream.ReadUint16();
                                }
                                else
                                {
                                    stream.Skip(2);
                                }
                            }

                            // Get blend mode and color alpha.
                            float colorAlpha = 1.0f;
                            GetTmdBlendModeAndColorAlpha(blendMode, colorAlpha,
                                                         mode & (int)TmdPrimitiveModes::Transparency,
                                                         (TmdBlendMode)((tsb & (int)TmdTextureAttribs::BlendMode) >> 5));

                            // Set colors.
                            for (auto& color : colors)
                            {
                                color = Color(1.0f, 1.0f, 1.0f, colorAlpha);
                            }

                            // Set tpage.
                            tpage = tsb & (int)TmdTextureAttribs::TPage;
                        }
                        // Read untextured polygon.
                        else
                        {
                            // Get blend mode and color alpha.
                            float colorAlpha = 1.0f;
                            if (mode & (int)TmdPrimitiveModes::Transparency)
                            {
                                GetTmdBlendModeAndColorAlpha(blendMode, colorAlpha, true, TmdBlendMode::AlphaHalf);
                            }

                            // Read colors.
                            uint32 color = stream.ReadUint32();
                            for (int i = 0; i < vertCount; i++)
                            {
                                if (isGouraud && i > 0)
                                {
                                    color = stream.ReadUint32();
                                }

                                colors[i] = ConvertTmdVertexColor(color, colorAlpha);
                            }
                        }

                        // Read position indices.
                        auto posIdxs = std::vector<uint16>(vertCount);
                        for (auto& idx : posIdxs)
                        {
                            idx = stream.ReadUint16();
                        }
                        if (isTri)
                        {
                            stream.Skip(2);
                        }

                        // Read normal indices.
                        auto   normalIdxs = std::vector<uint16>(vertCount);
                        uint16 normalIdx  = stream.ReadUint16();
                        for (int i = 0; i < vertCount; i++)
                        {
                            if (isGouraud && i > 0) 
                            {
                                normalIdx = stream.ReadUint16();
                            }

                            normalIdxs[i] = normalIdx;
                        }
                        if (!isGouraud || isTri)
                        {
                            stream.Skip(2);
                        }

                        // Collect primitive.
                        auto prim = TmdPrimitive
                        {
                            .BlendMd = blendMode,
                            .TPage   = tpage
                        };
                        for (int i = 0; i < vertCount; i++)
                        {
                            prim.Vertices.push_back(TmdVertex
                            {
                                .PositionIdx = posIdxs[i],
                                .NormalIdx   = normalIdxs[i],
                                .UvIdx       = GetLookupIdx(uvLookup, uvs[i]),
                                .ColorIdx    = GetLookupIdx(colorLookup, colors[i])
                            });
                        }
                        mesh.Primitives.push_back(prim);
                        break;
                    }
                    case TmdPrimitiveType::Line:
                    case TmdPrimitiveType::Sprite:
                    {
                        Debug::Log(Fmt("Attempted to read unsupported primitive type while parsing TMD `{}`.",
                                       filename.string()),
                                   Debug::LogLevel::Warning);
                        break;
                    }
                }

                stream.SetPosition(nextPrimPos);
            }

            // Collect indexed UVs.
            mesh.Uvs.resize(uvLookup.size());
            for (const auto& [keyUv, uvIdx] : uvLookup)
            {
                mesh.Uvs[uvIdx] = keyUv;
            }

            // Collect indexed colors.
            mesh.Colors.resize(colorLookup.size());
            for (const auto& [keyColor, colorIdx] : colorLookup)
            {
                mesh.Colors[colorIdx] = keyColor;
            }

            // Collect mesh.
            asset.Meshes.push_back(std::move(mesh));
        }

        /*for (auto& mesh : asset.Meshes)
        {
            float radius = 0.0f;
            int i = 0;
            for (const auto& prim : mesh.Primitives)
            {
                for (const auto& vert : prim.Vertices)
                {
                    auto pos = mesh.Positions[vert.PositionIdx];
                    auto normal = mesh.Normals[std::min<int>(vert.NormalIdx, mesh.Normals.size() - 1)];
                    auto uv = mesh.Uvs[vert.UvIdx];
                    auto color = mesh.Colors[vert.ColorIdx];
                    mesh.Linear.Vertices.push_back(BufferVertex3d
                    {
                        .Position = pos / 5792.61865f,
                        .Normal   = normal,
                        .Uv       = uv,
                        .Col      = color
                    });

                    radius = std::max(radius, Vector3::Distance(Vector3::Zero, pos));
                }
                i++;

                // Collect linear vertex indices.
                if (prim.Vertices.size() == TRI_IDX_COUNT)
                {
                    mesh.Linear.Idxs.insert(mesh.Linear.Idxs.end(),
                    {
                        (uint16)mesh.Linear.Vertices.size() - 2, (uint16)mesh.Linear.Vertices.size() - 1, (uint16)mesh.Linear.Vertices.size()
                    });
                }
                else if (prim.Vertices.size() == QUAD_IDX_COUNT)
                {
                    mesh.Linear.Idxs.insert(mesh.Linear.Idxs.end(),
                    {
                        (uint16)mesh.Linear.Vertices.size() - 2, (uint16)mesh.Linear.Vertices.size() - 1, (uint16)mesh.Linear.Vertices.size(),
                        (uint16)mesh.Linear.Vertices.size() - 2, (uint16)mesh.Linear.Vertices.size(), (uint16)mesh.Linear.Vertices.size() - 3
                    });
                }
            }
        }*/

        // Convert to linear meshes. @todo Implement render buckets? Sort primitives by CLUT?
        for (auto& mesh : asset.Meshes)
        {
            // Run through primitives.
            auto vertLookup = std::unordered_map<TmdVertex, int>{};
            for (const auto& prim : mesh.Primitives)
            {
                // Collect primitive vertex indices.
                auto primIdxs = std::vector<uint16>{};
                for (const auto& vert : prim.Vertices)
                {
                    uint16 newIdx = GetLookupIdx(vertLookup, vert);
                    primIdxs.push_back(newIdx);
                }

                // Collect linear vertex indices.
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

            // Collect linear indexed vertices.
            mesh.Linear.Vertices.resize(vertLookup.size());
            for (const auto& [keyVert, vertIdx] : vertLookup)
            {
                mesh.Linear.Vertices[vertIdx] = BufferVertex3d
                {
                    .Position = mesh.Positions[keyVert.PositionIdx] / 4096.0f,
                    .Normal   = mesh.Normals[std::min<int>(keyVert.NormalIdx, mesh.Normals.size() - 1)],
                    .Uv       = mesh.Uvs[keyVert.UvIdx],
                    .Col      = mesh.Colors[keyVert.ColorIdx]
                };
            }
        }

        return std::make_shared<TmdAsset>(std::move(asset));
    }
}
