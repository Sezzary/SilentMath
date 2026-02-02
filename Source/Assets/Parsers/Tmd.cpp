#include "Framework.h"
#include "Assets/Parsers/Tmd.h"

#include "Renderer/Common/Enums.h"
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
        Tpage        = (1 << 0) | (1 << 2) | (1 << 3) | (1 << 4), /** Unused. */
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

    /** @brief TMD header. */
    struct TmdHeader
    {
        uint32 Version   = 0; /** Unused. */
        uint32 Flags     = 0;
        uint32 MeshCount = 0;
    };

    /** @brief TMD mesh description. */
    struct TmdMeshDesc
    {
        uint32 VertexOffset    = 0;
        uint32 VertexCount     = 0;
        uint32 NormalOffset    = 0;
        uint32 NormalCount     = 0;
        uint32 PrimitiveOffset = 0;
        uint32 PrimitiveCount  = 0;
        uint32 Scale           = 0; /** Unused. */
    };

    /** @brief TMD primitive attributes. */
    struct TmdPrimitiveAttribs
    {
        int8 Olen  = 0; /** Unused. */
        int8 Ilen  = 0; /** Packet size in words. */
        int8 Flags = 0; /** `TmdPrimitiveFlags` */
        int8 Mode  = 0; /** `TmdPrimitiveModes` */
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
                               FP_COLOR_FROM(colorAlpha));
    }

    /** @brief Gets the blend mode and color alpha of a TMD primitive.
     *
     * @param hasTransparency Primitive is transparent.
     * @param tmdBlendMode TMD blend mode.
     * @param blendMode Output blend mode.
     * @param colorAlpha Output color alpha.
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
        // Read file.
        auto stream = Stream(filename, true, false);
        if (!stream.IsOpen())
        {
            throw std::runtime_error(Fmt("Failed to open TMD `{}`.", filename.string()));
        }

        // Read header.
        auto header = TmdHeader
        {
            .Version   = stream.ReadUint32(),
            .Flags     = stream.ReadUint32(),
            .MeshCount = stream.ReadUint32()
        };

        // Compute base data address.
        int baseAddr = sizeof(TmdHeader) + (header.MeshCount * sizeof(TmdMeshDesc));

        // Read mesh descriptions.
        auto meshDescs = std::vector<TmdMeshDesc>(header.MeshCount);
        for (auto& meshDesc : meshDescs)
        {
            // Read vertex data.
            meshDesc.VertexOffset = stream.ReadUint32();
            meshDesc.VertexCount  = stream.ReadUint32();

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
                meshDesc.VertexOffset    -= baseAddr;
                meshDesc.NormalOffset    -= baseAddr;
                meshDesc.PrimitiveOffset -= baseAddr;
            }
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
            auto&       mesh     = asset.Meshes[i];

            // Create UV and color lookups.
            auto uvLookup    = std::unordered_map<Vector2, int>{}; // Key = UV, value = UV index.
            auto colorLookup = std::unordered_map<Color,   int>{}; // Key = color, value = color index.

            // Read vertex positions.
            stream.Seek(baseAddr + meshDesc.VertexOffset);
            mesh.Positions.reserve(meshDesc.VertexCount);
            for (int j = 0; j < meshDesc.VertexCount; j++)
            {
                // Read components.
                int16 x = stream.ReadInt16();
                int16 y = stream.ReadInt16();
                int16 z = stream.ReadInt16();
                stream.ReadInt16(); // Padding.

                // Collect position.
                mesh.Positions.push_back(Vector3(x, y, z));
            }

            // Read vertex normals.
            stream.Seek(baseAddr + meshDesc.NormalOffset);
            mesh.Normals.reserve(meshDesc.NormalCount);
            for (int j = 0; j < meshDesc.NormalCount; j++)
            {
                // Read components.
                int16 x = stream.ReadInt16();
                int16 y = stream.ReadInt16();
                int16 z = stream.ReadInt16();
                stream.ReadInt16(); // Padding.

                // Collect normalized normal.
                auto normal = Vector3::Normalize(Vector3(x, y, z));
                mesh.Normals.push_back(normal);
            }

            // Read primitives.
            stream.Seek(baseAddr + meshDesc.PrimitiveOffset);
            mesh.Primitives.reserve(meshDesc.PrimitiveCount);
            for (int j = 0; j < meshDesc.PrimitiveCount; j++)
            {
                // Read attributes.
                auto attribs = TmdPrimitiveAttribs
                {
                    .Olen  = stream.ReadInt8(),
                    .Ilen  = stream.ReadInt8(),
                    .Flags = stream.ReadInt8(),
                    .Mode  = stream.ReadInt8()
                };

                // Compute next primitive position.
                int nextPrimPos = stream.GetPosition() + (attribs.Ilen * sizeof(int32));

                // Read primitive.
                auto primType = (TmdPrimitiveType)((attribs.Mode & (int)TmdPrimitiveModes::Primitive) >> 5);
                switch (primType)
                {
                    case TmdPrimitiveType::Polygon:
                    {
                        // Read quad/triangle attributes.
                        if (attribs.Mode & (int)TmdPrimitiveModes::Quad)
                        {
                            // Read vertex UVs, colors, and blend mode.
                            auto uvs       = std::array<Vector2, QUAD_VERTEX_COUNT>{};
                            auto colors    = std::array<Color,   QUAD_VERTEX_COUNT>{};
                            auto blendMode = BlendMode::Opaque;
                            if (attribs.Mode & (int)TmdPrimitiveModes::Textured)
                            {
                                // Read UV0.
                                uint8 u0 = stream.ReadUint8();
                                uint8 v0 = stream.ReadUint8();

                                // Read CLUT position attributes (unused).
                                uint16 cba = stream.ReadUint16();

                                // Read UV1.
                                uint8 u1 = stream.ReadUint8();
                                uint8 v1 = stream.ReadUint8();

                                // Read texture page attributes.
                                uint16 tsb = stream.ReadUint16();

                                // Read UV2.
                                uint8 u2 = stream.ReadUint8();
                                uint8 v2 = stream.ReadUint8();
                                stream.ReadUint16(); // Padding.

                                // Read UV3.
                                uint8 u3 = stream.ReadUint8();
                                uint8 v3 = stream.ReadUint8();
                                stream.ReadUint16(); // Padding.

                                // Set normalized UVs.
                                uvs =
                                {
                                    Vector2(u0, v0) / UCHAR_MAX,
                                    Vector2(u1, v1) / UCHAR_MAX,
                                    Vector2(u2, v2) / UCHAR_MAX,
                                    Vector2(u3, v3) / UCHAR_MAX
                                };

                                // Get blend mode and color alpha.
                                float colorAlpha = 0.0f;
                                GetTmdBlendModeAndColorAlpha(blendMode, colorAlpha,
                                                             attribs.Mode & (int)TmdPrimitiveModes::Transparency,
                                                             (TmdBlendMode)((tsb & (int)TmdTextureAttribs::BlendMode) >> 5));

                                // Set colors.
                                colors =
                                {
                                    Color(1.0f, 1.0f, 1.0f, colorAlpha),
                                    Color(1.0f, 1.0f, 1.0f, colorAlpha),
                                    Color(1.0f, 1.0f, 1.0f, colorAlpha),
                                    Color(1.0f, 1.0f, 1.0f, colorAlpha)
                                };
                            }
                            else
                            {
                                // Read colors.
                                uint32 color0 = 0;
                                uint32 color1 = 0;
                                uint32 color2 = 0;
                                uint32 color3 = 0;
                                if (attribs.Mode & (int)TmdPrimitiveModes::Gouraud)
                                {
                                    color0 = stream.ReadUint32();
                                    color1 = stream.ReadUint32();
                                    color2 = stream.ReadUint32();
                                    color3 = stream.ReadUint32();
                                }
                                else
                                {
                                    color0 =
                                    color1 =
                                    color2 =
                                    color3 = stream.ReadUint32();
                                }

                                // Set blend mode and colors.
                                if (attribs.Mode & (int)TmdPrimitiveModes::Transparency)
                                {
                                    float colorAlpha = 0.0f;
                                    GetTmdBlendModeAndColorAlpha(blendMode, colorAlpha, true, TmdBlendMode::AlphaHalf);

                                    colors =
                                    {
                                        ConvertTmdVertexColor(color0, colorAlpha),
                                        ConvertTmdVertexColor(color1, colorAlpha),
                                        ConvertTmdVertexColor(color2, colorAlpha),
                                        ConvertTmdVertexColor(color3, colorAlpha)
                                    };
                                }
                                else
                                {
                                    blendMode = BlendMode::Opaque;
                                    colors    = 
                                    {
                                        ConvertTmdVertexColor(color0, 1.0f),
                                        ConvertTmdVertexColor(color1, 1.0f),
                                        ConvertTmdVertexColor(color2, 1.0f),
                                        ConvertTmdVertexColor(color2, 1.0f)
                                    };
                                }
                            }

                            // Read vertex position indices.
                            uint16 posIdx0 = stream.ReadUint16();
                            uint16 posIdx1 = stream.ReadUint16();
                            uint16 posIdx2 = stream.ReadUint16();
                            uint16 posIdx3 = stream.ReadUint16();

                            // Read vertex normal indices.
                            uint16 normalIdx0 = 0;
                            uint16 normalIdx1 = 0;
                            uint16 normalIdx2 = 0;
                            uint16 normalIdx3 = 0;
                            if (attribs.Mode & (int)TmdPrimitiveModes::Gouraud)
                            {
                                normalIdx0 = stream.ReadUint16();
                                normalIdx1 = stream.ReadUint16();
                                normalIdx2 = stream.ReadUint16();
                                normalIdx3 = stream.ReadUint16();
                            }
                            else
                            {
                                normalIdx0 =
                                normalIdx1 =
                                normalIdx2 =
                                normalIdx3 = stream.ReadUint16();
                                stream.ReadUint16(); // Padding.
                            }

                            // Collect quad.
                            mesh.Primitives.push_back(TmdPrimitive
                            {
                                .Vertices =
                                {
                                    TmdVertex{ posIdx0, normalIdx0, GetLookupIdx(uvLookup, uvs[0]), GetLookupIdx(colorLookup, colors[0]) },
                                    TmdVertex{ posIdx1, normalIdx1, GetLookupIdx(uvLookup, uvs[1]), GetLookupIdx(colorLookup, colors[1]) },
                                    TmdVertex{ posIdx2, normalIdx2, GetLookupIdx(uvLookup, uvs[2]), GetLookupIdx(colorLookup, colors[2]) },
                                    TmdVertex{ posIdx3, normalIdx3, GetLookupIdx(uvLookup, uvs[3]), GetLookupIdx(colorLookup, colors[3]) }
                                },
                                .BlendMd = blendMode
                            });
                        }
                        else
                        {
                            // Read vertex UVs, colors, and blend mode.
                            auto uvs       = std::array<Vector2, TRI_VERTEX_COUNT>{};
                            auto colors    = std::array<Color,   TRI_VERTEX_COUNT>{};
                            auto blendMode = BlendMode::Opaque;
                            if (attribs.Mode & (int)TmdPrimitiveModes::Textured)
                            {
                                // Read UV0.
                                uint8 u0 = stream.ReadUint8();
                                uint8 v0 = stream.ReadUint8();

                                // Read CLUT position attributes (unused).
                                uint16 cba = stream.ReadUint16();

                                // Read UV1.
                                uint8 u1 = stream.ReadUint8();
                                uint8 v1 = stream.ReadUint8();

                                // Read texture page attributes.
                                uint16 tsb = stream.ReadUint16();

                                // Read UV2.
                                uint8 u2 = stream.ReadUint8();
                                uint8 v2 = stream.ReadUint8();
                                stream.ReadUint16(); // Padding.

                                // Set normalized UVs.
                                uvs =
                                {
                                    Vector2(u0, v0) / UCHAR_MAX,
                                    Vector2(u1, v1) / UCHAR_MAX,
                                    Vector2(u2, v2) / UCHAR_MAX
                                };

                                // Get blend mode and color alpha.
                                float colorAlpha = 0.0f;
                                GetTmdBlendModeAndColorAlpha(blendMode, colorAlpha,
                                                             attribs.Mode & (int)TmdPrimitiveModes::Transparency,
                                                             (TmdBlendMode)((tsb & (int)TmdTextureAttribs::BlendMode) >> 5));

                                // Set colors.
                                colors =
                                {
                                    Color(1.0f, 1.0f, 1.0f, colorAlpha),
                                    Color(1.0f, 1.0f, 1.0f, colorAlpha),
                                    Color(1.0f, 1.0f, 1.0f, colorAlpha)
                                };
                            }
                            else
                            {
                                // Read colors.
                                uint32 color0 = 0;
                                uint32 color1 = 0;
                                uint32 color2 = 0;
                                if (attribs.Mode & (int)TmdPrimitiveModes::Gouraud)
                                {
                                    color0 = stream.ReadUint32();
                                    color1 = stream.ReadUint32();
                                    color2 = stream.ReadUint32();
                                }
                                else
                                {
                                    color0 =
                                    color1 =
                                    color2 = stream.ReadUint32();
                                }

                                // Set blend mode and colors.
                                if (attribs.Mode & (int)TmdPrimitiveModes::Transparency)
                                {
                                    float colorAlpha = 0.0f;
                                    GetTmdBlendModeAndColorAlpha(blendMode, colorAlpha, true, TmdBlendMode::AlphaHalf);

                                    // Set normalized colors.
                                    colors =
                                    {
                                        ConvertTmdVertexColor(color0, colorAlpha),
                                        ConvertTmdVertexColor(color1, colorAlpha),
                                        ConvertTmdVertexColor(color2, colorAlpha)
                                    };
                                }
                                else
                                {
                                    blendMode = BlendMode::Opaque;
                                    colors    = 
                                    {
                                        ConvertTmdVertexColor(color0, 1.0f),
                                        ConvertTmdVertexColor(color1, 1.0f),
                                        ConvertTmdVertexColor(color2, 1.0f)
                                    };
                                }
                            }

                            // Read vertex position indices.
                            uint16 posIdx0 = stream.ReadUint16();
                            uint16 posIdx1 = stream.ReadUint16();
                            uint16 posIdx2 = stream.ReadUint16();
                            stream.ReadUint16(); // Padding.

                            // Read vertex normal indices.
                            uint16 normalIdx0 = 0;
                            uint16 normalIdx1 = 0;
                            uint16 normalIdx2 = 0;
                            if (attribs.Mode & (int)TmdPrimitiveModes::Gouraud)
                            {
                                normalIdx0 = stream.ReadUint16();
                                normalIdx1 = stream.ReadUint16();
                                normalIdx2 = stream.ReadUint16();
                                stream.ReadUint16(); // Padding.
                            }
                            else
                            {
                                normalIdx0 =
                                normalIdx1 =
                                normalIdx2 = stream.ReadUint16();
                                stream.ReadUint16(); // Padding.
                            }

                            // Collect triangle.
                            mesh.Primitives.push_back(TmdPrimitive
                            {
                                .Vertices =
                                {
                                    TmdVertex{ posIdx0, normalIdx0, GetLookupIdx(uvLookup, uvs[0]), GetLookupIdx(colorLookup, colors[0]) },
                                    TmdVertex{ posIdx1, normalIdx1, GetLookupIdx(uvLookup, uvs[1]), GetLookupIdx(colorLookup, colors[1]) },
                                    TmdVertex{ posIdx2, normalIdx2, GetLookupIdx(uvLookup, uvs[2]), GetLookupIdx(colorLookup, colors[2]) }
                                },
                                .BlendMd = blendMode
                            });
                        }
                        break;
                    }
                    case TmdPrimitiveType::Line:
                    case TmdPrimitiveType::Sprite:
                    {
                        // Unsupported.
                        break;
                    }
                }

                stream.Seek(nextPrimPos);
            }

            // Copy indexed UVs.
            mesh.Uvs.resize(uvLookup.size());
            for (const auto& [keyUv, uvIdx] : uvLookup)
            {
                mesh.Uvs[uvIdx] = keyUv;
            }

            // Copy indexed colors.
            mesh.Colors.resize(colorLookup.size());
            for (const auto& [keycolor, colorIdx] : colorLookup)
            {
                mesh.Colors[colorIdx] = keycolor;
            }
        }

        // @todo Sort primitives by CLUT for efficient batching when rendering. CLUT can be interpreted in a shader.

        return std::make_shared<TmdAsset>(std::move(asset));
    }
}
