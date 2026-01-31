#include "Framework.h"
#include "Assets/Parsers/Tmd.h"

#include "Utils/Stream.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Assets
{
    enum class TmdMeshFlags
    {
        LightSource = 1 << 0,
        TwoSided    = 1 << 1,
        Gradation   = 1 << 2
    };

    enum class TmdModes
    {
        Brightness      = 1 << 0,
        SemiTransparent = 1 << 1,
        Textured        = 1 << 2,
        Quad            = 1 << 3,                         /** 0: triangle, 1: quad. */
        Gouraud         = 1 << 4,                         /** 0: flat, 1: gouraud. */
        Primitive       = (1 << 5) | (1 << 6) | (1 << 7)  /** Spans 3 bits. 1: polygon (triangle/quad), 2: line, 3: sprite. */
    };

    /** TMD primitive types. */
    enum class TmdPrimitiveType
    {
        Polygon = 1,
        Line    = 2,
        Sprite  = 3
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
        uint32 Scale           = 0;
    };

    std::shared_ptr<void> ParseTmd(const std::filesystem::path& filename)
    {
        constexpr int HDR_SIZE      = sizeof(uint32) * 3;
        constexpr int MESH_HDR_SIZE = sizeof(TmdMesh);
        constexpr int FIXP_FLAG     = 1 << 0;

        // Read file.
        auto stream = Stream(filename, true, false);
        if (!stream.IsOpen())
        {
            throw std::runtime_error(Fmt("Failed to open TMD `{}`.", filename.string()));
        }

        // Read header.
        uint32 version   = stream.ReadUint32(); // Unused.
        uint32 flags     = stream.ReadUint32();
        uint32 meshCount = stream.ReadUint32();

        // Compute base data address.
        int baseAddr = HDR_SIZE + (meshCount * MESH_HDR_SIZE);

        // Read meshes.
        auto meshDescs = std::vector<TmdMeshDesc>(meshCount);
        for (auto& mDescesh : meshDescs)
        {
            // Read vertex data.
            mDescesh.VertexOffset = stream.ReadUint32();
            mDescesh.VertexCount  = stream.ReadUint32();

            // Read normal data.
            mDescesh.NormalOffset = stream.ReadUint32();
            mDescesh.NormalCount  = stream.ReadUint32();

            // Read primitive data.
            mDescesh.PrimitiveOffset = stream.ReadUint32();
            mDescesh.PrimitiveCount  = stream.ReadUint32();

            // Read scale (unused).
            mDescesh.Scale = stream.ReadUint32();

            if (flags & FIXP_FLAG)
            {
                mDescesh.VertexOffset    -= baseAddr;
                mDescesh.NormalOffset    -= baseAddr;
                mDescesh.PrimitiveOffset -= baseAddr;
            }
        }

        // Create asset.
        auto asset = TmdAsset
        {
            .Meshes = std::vector<TmdMesh>(meshCount)
        };

        // Read meshes.
        for (int i = 0; i < meshCount; i++)
        {
            const auto& meshDesc = meshDescs[i];
            auto&       mesh     = asset.Meshes[i];

            // Read vertices.
            stream.Seek(baseAddr + meshDesc.VertexOffset);
            mesh.Vertices.reserve(meshDesc.VertexCount);
            for (int j = 0; j < meshDesc.VertexCount; j++)
            {
                // Read components.
                int16 x = stream.ReadInt16();
                int16 y = stream.ReadInt16();
                int16 z = stream.ReadInt16();
                stream.ReadInt16(); // Padding.

                // Collect vertex.
                mesh.Vertices.push_back(Vector3(x, y, z));
            }

            // Read normals.
            stream.Seek(baseAddr + meshDesc.NormalOffset);
            mesh.Normals.reserve(meshDesc.NormalCount);
            for (int j = 0; j < meshDesc.NormalCount; j++)
            {
                // Read components.
                int16 x = stream.ReadInt16();
                int16 y = stream.ReadInt16();
                int16 z = stream.ReadInt16();
                stream.ReadInt16(); // Padding.

                // Collect normal.
                auto normal = Vector3::Normalize(Vector3(x, y, z));
                mesh.Normals.push_back(normal);
            }

            // Read primitives.
            stream.Seek(baseAddr + meshDesc.PrimitiveOffset);
            mesh.Primitives.reserve(meshDesc.PrimitiveCount);
            for (int j = 0; j < meshDesc.PrimitiveCount; j++)
            {
                // Read attributes.
                int8 olen  = stream.ReadInt8();
                int8 ilen  = stream.ReadInt8();
                int8 flags = stream.ReadInt8();
                int8 mode  = stream.ReadInt8();

                // Compute next primitive position.
                int nextPrimPos = stream.GetPosition() + (ilen * 4);

                // Read polygon.
                auto primType = (TmdPrimitiveType)((mode & (int)TmdModes::Primitive) >> 5);
                switch (primType)
                {
                    case TmdPrimitiveType::Polygon:
                    {
                        // @todo
                        if (mode & (int)TmdModes::Textured)
                        {
                            uint8  u0  = stream.ReadUint8();
                            uint8  v0  = stream.ReadUint8();
                            uint16 cba = stream.ReadUint16(); // CLUT attribute.

                            uint8  u1  = stream.ReadUint8();
                            uint8  v1  = stream.ReadUint8();
                            uint16 tsb = stream.ReadUint16(); // Texture Page attribute.

                            uint8 u2 = stream.ReadUint8();
                            uint8 v2 = stream.ReadUint8();
                            stream.ReadUint16(); // Padding.

                            if (mode & (int)TmdModes::Quad)
                            {
                                uint8 u3 = stream.ReadUint8();
                                uint8 v3 = stream.ReadUint8();
                                stream.ReadUint16(); // Padding.
                            }
                        }
                        else
                        {
                            // Read first color.
                            uint32 color0 = stream.ReadUint32(); 

                            if (mode & (int)TmdModes::Gouraud)
                            {
                                uint32 color1 = stream.ReadUint32();
                                uint32 color2 = stream.ReadUint32();
                                if (mode & (int)TmdModes::Quad)
                                {
                                    uint32 color3 = stream.ReadUint32();
                                }
                            }
                        }

                        if (mode & (int)TmdModes::Quad)
                        {
                            // Read vertex indices.
                            uint16 vertIdx0 = stream.ReadUint16();
                            uint16 vertIdx1 = stream.ReadUint16();
                            uint16 vertIdx2 = stream.ReadUint16();
                            uint16 vertIdx3 = stream.ReadUint16();

                            // Read normal indices.
                            uint16 normalIdx0 = 0;
                            uint16 normalIdx1 = 0;
                            uint16 normalIdx2 = 0;
                            uint16 normalIdx3 = 0;
                            if (mode & (int)TmdModes::Gouraud)
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
                            }

                            // Collect quad.
                            mesh.Primitives.push_back(TmdQuad
                            {
                                .VertexIdxs = { vertIdx0,vertIdx1, vertIdx2, vertIdx3 },
                                .NormalIdxs = { normalIdx0, normalIdx1, normalIdx2, normalIdx3 }
                            });
                        }
                        else
                        {
                            // Read vertex indices.
                            uint16 vertIdx0 = stream.ReadUint16();
                            uint16 vertIdx1 = stream.ReadUint16();
                            uint16 vertIdx2 = stream.ReadUint16();

                            // Read normal indices.
                            uint16 normalIdx0 = 0;
                            uint16 normalIdx1 = 0;
                            uint16 normalIdx2 = 0;
                            if (mode & (int)TmdModes::Gouraud)
                            {
                                normalIdx0 = stream.ReadUint16();
                                normalIdx1 = stream.ReadUint16();
                                normalIdx2 = stream.ReadUint16();
                            }
                            else
                            {
                                normalIdx0 =
                                normalIdx1 =
                                normalIdx2 = stream.ReadUint16();
                            }

                            // Collect triangle.
                            mesh.Primitives.push_back(TmdTriangle
                            {
                                .VertexIdxs = { vertIdx0, vertIdx1, vertIdx2 },
                                .NormalIdxs = { normalIdx0, normalIdx1, normalIdx2 }
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
        }

        return std::make_shared<TmdAsset>(std::move(asset));
    }
}
