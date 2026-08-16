#include "Framework.h"
#include "Assets/Loaders/Tmd.h"

#include "Application.h"
#include "Assets/Loaders/Utils/LinearMesh.h"
#include "Renderer/Common/Enums.h"
#include "Renderer/Common/Resources/Layouts/Buffers.h"
#include "Renderer/Renderer.h"
#include "Utils/Stream.h"
#include "Utils/Utils.h"

using namespace Silent::Renderer;
using namespace Silent::Utils;

namespace Silent::Assets
{
    /** @brief TMD header flags. */
    enum class TmdHeaderFlags
    {
        Fixp = 1 << 0 /** 0: Relative offset from meshes block start, 1: Absolute offset from file start. */
    };

    /** @brief TMD primitive flags. */
    enum class TmdPrimitiveFlags
    {
        Lit         = 1 << 0, /** 0: Use light, 1: Use raw color. */
        DoubleSided = 1 << 1, /** 0: Single-sided, 1: Double-sided. */
        Graded      = 1 << 2  /** Untextured only. 0: Solid color, 1: Graded color. */
    };

    /** @brief TMD packed primitive modes. */
    enum class TmdPrimitiveModes
    {
        Brightness   = 1 << 0,                         /** 0: Brightly lit, 1: Textured as-is. */
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
        ColorMode    = (1 << 7) | (1 << 8)                        /** 0: 4-bit, 1: 8-bit, 2: 15-bit. */
    };

    /** TMD primitive types. */
    enum class TmdPrimitiveType
    {
        Polygon = 1,
        Line    = 2,
        Sprite  = 3
    };

    /** @brief TMD primitive blend modes. */
    enum class TmdBlendMode
    {
        AlphaHalf  = 0,
        Add        = 1,
        Subtract   = 2,
        AddQuarter = 3
    };

    /** @brief TMD header. */
    struct TmdHeader
    {
        uint32 Version   = 0; /** Unused. */
        uint32 Flags     = 0;
        uint32 MeshCount = 0;
    };

    /** @brief TMD mesh description. */
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

    std::shared_ptr<void> ParseTmd(const stdfs::path& filename)
    {
        const auto& fs = g_App.GetFilesystem();

        // Read file.
        auto stream = Stream(filename, true, false);
        if (!stream.IsOpen())
        {
            throw std::runtime_error(Fmt("Failed to open TMD `{}`.",
                                         stdfs::relative(filename, fs.GetAssetsDirectory()).string()));
        }

        // Read header.
        auto header = TmdHeader
        {
            .Version   = stream.ReadUint32(),
            .Flags     = stream.ReadUint32(),
            .MeshCount = stream.ReadUint32()
        };

        // Compute meshes offset.
        int  meshesOffset = sizeof(TmdHeader) + (header.MeshCount * sizeof(TmdMeshDescLayout));
        bool hasFixp      = header.Flags & (int)TmdHeaderFlags::Fixp;

        // Read mesh descriptions.
        auto meshDescs = std::vector<TmdMeshDescLayout>{};
        meshDescs.reserve(header.MeshCount);
        for (int i = 0; i < header.MeshCount; i++)
        {
            // Read mesh description.
            auto meshDesc = TmdMeshDescLayout
            {
                .PositionOffset  = stream.ReadUint32(),
                .PositionCount   = stream.ReadUint32(),
                .NormalOffset    = stream.ReadUint32(),
                .NormalCount     = stream.ReadUint32(),
                .PrimitiveOffset = stream.ReadUint32(),
                .PrimitiveCount  = stream.ReadUint32(),
                .Scale           = stream.ReadUint32()
            };

            // Collect mesh description.
            meshDescs.push_back(std::move(meshDesc));
        }

        // Read meshes.
        auto meshes = std::vector<TmdMesh>(header.MeshCount);
        for (int i = 0; i < header.MeshCount; i++)
        {
            const auto& meshDesc = meshDescs[i];

            auto mesh = TmdMesh{};

            // @todo Texture name?

            // Create UV and color index lookups.
            auto uvLookup    = std::unordered_map<Vector2i, int>{}; // Key = UV, value = UV index.
            auto colorLookup = std::unordered_map<Color,    int>{}; // Key = color, value = color index.

            // Set stream position to vertex positions.
            stream.SetPosition(hasFixp ? meshDesc.PositionOffset : (meshesOffset + meshDesc.PositionOffset));

            // Read vertex positions.
            mesh.Native.Positions.reserve(meshDesc.PositionCount);
            for (int j = 0; j < meshDesc.PositionCount; j++)
            {
                // Read components.
                q3_12 x = stream.ReadInt16();
                q3_12 y = stream.ReadInt16();
                q3_12 z = stream.ReadInt16();

                stream.Skip(2);

                // Collect position.
                mesh.Native.Positions.push_back(Vector3i(x, y, z));
            }

            // Set stream position to vertex normals.
            stream.SetPosition(hasFixp ? meshDesc.NormalOffset : (meshesOffset + meshDesc.NormalOffset));

            // Read vertex normals.
            mesh.Native.Normals.reserve(meshDesc.NormalCount);
            for (int j = 0; j < meshDesc.NormalCount; j++)
            {
                // Read components.
                q3_12 x = stream.ReadInt16();
                q3_12 y = stream.ReadInt16();
                q3_12 z = stream.ReadInt16();

                stream.Skip(2);

                // Collect normal.
                auto normal = Vector3i(x, y, z);
                mesh.Native.Normals.push_back(normal);
            }

            // Set stream position to primitives.
            stream.SetPosition(hasFixp ? meshDesc.PrimitiveOffset : (meshesOffset + meshDesc.PrimitiveOffset));

            // Read primitives.
            mesh.Native.Primitives.reserve(meshDesc.PrimitiveCount);
            for (int j = 0; j < meshDesc.PrimitiveCount; j++)
            {
                // Read attributes.
                uint8 olen  = stream.ReadUint8(); // Unused.
                uint8 ilen  = stream.ReadUint8(); // Packet size in words.
                uint8 flags = stream.ReadUint8(); // `TmdPrimitiveFlags`
                uint8 mode  = stream.ReadUint8(); // `TmdPrimitiveModes`

                bool isGraded      = flags & (int)TmdPrimitiveFlags::Graded;
                bool isBright      = mode & (int)TmdPrimitiveModes::Brightness;
                bool isTransparent = mode & (int)TmdPrimitiveModes::Transparency;
                bool isTextured    = mode & (int)TmdPrimitiveModes::Textured;
                bool isQuad        = mode & (int)TmdPrimitiveModes::Quad;
                bool isGouraud     = mode & (int)TmdPrimitiveModes::Gouraud;
                auto primType      = (TmdPrimitiveType)((mode & (int)TmdPrimitiveModes::Primitive) >> 5);

                // Store next primitive offset.
                int nextPrimOffset = stream.GetPosition() + (ilen * sizeof(int32));

                // Read primitive.
                switch (primType)
                {
                    case TmdPrimitiveType::Polygon:
                    {
                        // Read attributes.
                        int   vertCount  = isQuad ? QUAD_VERTEX_COUNT : TRI_VERTEX_COUNT;
                        auto  uvs        = std::vector<Vector2i>(vertCount, Vector2i::Zero);
                        auto  colors     = std::vector<Color>(vertCount, Color::White);
                        auto  blendMode  = BlendMode::Opaque;
                        float colorAlpha = 1.0f;
                        int   tPage      = 0;
                        bool  isTri      = vertCount == TRI_VERTEX_COUNT;

                        // Read textured polygon UVs.
                        if (isTextured)
                        {
                            // Read UVs, CBA, and TSB.
                            uint16 cba = 0; // Unused.
                            uint16 tsb = 0;
                            for (int i = 0; i < vertCount; i++)
                            {
                                // Read UV.
                                uint8 u = stream.ReadUint8();
                                uint8 v = stream.ReadUint8();

                                // Set UV.
                                uvs[i] = Vector2i(u, v);

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

                            tPage             = tsb & (int)TmdTextureAttribs::TPage;
                            auto tmdBlendMode = (TmdBlendMode)((tsb & (int)TmdTextureAttribs::BlendMode) >> 5);
                            int colorMode     = (tsb & (int)TmdTextureAttribs::ColorMode) >> 7; // Unused.

                            // Get blend mode and color alpha.
                            GetTmdBlendModeAndColorAlpha(blendMode, colorAlpha, isTransparent, tmdBlendMode);

                            // Set colors.
                            for (auto& color : colors)
                            {
                                color = Color(1.0f, 1.0f, 1.0f, colorAlpha);
                            }
                        }
                        // Read untextured polygon colors.
                        else
                        {
                            // Get blend mode and color alpha.
                            if (isTransparent)
                            {
                                GetTmdBlendModeAndColorAlpha(blendMode, colorAlpha, true, TmdBlendMode::AlphaHalf);
                            }

                            // Read colors.
                            for (int i = 0; i < (isGraded ? vertCount : 1); i++)
                            {
                                uint32 color = stream.ReadUint32();
                                colors[i]    = ConvertTmdVertexColor(color, colorAlpha);
                            }
                        }

                        // Read normal indices.
                        auto normalIdxs = std::vector<uint16>(vertCount);
                        for (int i = 0; i < (isGouraud ? vertCount : 1); i++)
                        {
                            normalIdxs[i] = stream.ReadUint16();
                        }

                        // Read position indices.
                        auto posIdxs = std::vector<uint16>(vertCount);
                        for (auto& idx : posIdxs)
                        {
                            idx = stream.ReadUint16();
                        }

                        if (isQuad)
                        {
                            stream.Skip(2);
                        }

                        // Collect primitive.
                        auto prim = TmdPrimitive
                        {
                            .BlendMd = blendMode,
                            .TPage   = tPage
                        };
                        for (int i = 0; i < vertCount; i++)
                        {
                            prim.Vertices.push_back(TmdVertex
                            {
                                .PositionIdx = posIdxs[i],
                                .NormalIdx   = normalIdxs[isGouraud ? 0 : i],
                                .UvIdx       = GetLookupIdx(uvLookup, uvs[i]),
                                .ColorIdx    = GetLookupIdx(colorLookup, colors[i])
                            });
                        }
                        mesh.Native.Primitives.push_back(prim);
                        break;
                    }
                    default:
                    case TmdPrimitiveType::Line:
                    case TmdPrimitiveType::Sprite:
                    {
                        Debug::Log(Fmt("Attempted to read unsupported primitive type {} while parsing TMD `{}`.",
                                       (int)primType, filename.string()),
                                   Debug::LogLevel::Warning, Debug::LogMode::Debug);
                        break;
                    }
                }

                // Set stream position to next primitive.
                stream.SetPosition(nextPrimOffset);
            }

            // Collect indexed UVs.
            mesh.Native.Uvs.resize(uvLookup.size());
            for (const auto& [keyUv, uvIdx] : uvLookup)
            {
                mesh.Native.Uvs[uvIdx] = keyUv;
            }

            // Collect indexed colors.
            mesh.Native.Colors.resize(colorLookup.size());
            for (const auto& [keyColor, colorIdx] : colorLookup)
            {
                mesh.Native.Colors[colorIdx] = keyColor;
            }

            // Collect mesh.
            meshes.push_back(std::move(mesh));
        }

        // Convert to linear meshes. @todo Implement render buckets? Sort primitives by CLUT?
        /*for (auto& mesh : meshes)
        {
            // Run through primitives.
            auto vertLookup = std::unordered_map<TmdVertex, int>{};
            for (const auto& prim : mesh.Native.Primitives)
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
                    .Position = mesh.Native.Positions[keyVert.PositionIdx].ToVector3() / 4096.0f,
                    .Normal   = mesh.Native.Normals[keyVert.NormalIdx].ToVector3() / 4096.0f,
                    .Uv       = mesh.Native.Uvs[keyVert.UvIdx].ToVector2() / 255.0f, // @todo Unreliable.
                    .Col      = mesh.Native.Colors[keyVert.ColorIdx]
                };
            }
        }*/

        return std::make_shared<TmdAsset>(TmdAsset
        {
            .Meshes = std::move(meshes)
        });
    }

    void QueueTmdGpuUpload(const Asset& asset)
    {
        auto& renderer = g_App.GetRenderer();

        //renderer.QueueMeshUpload(asset.Name);
    }

    void QueueTmdGpuRelease(const Asset& asset)
    {
        auto& renderer = g_App.GetRenderer();

        //renderer.QueueMeshRelease(asset.Name);
    }
}
