#include "Framework.h"
#include "Assets/Parsers/Tmd.h"

#include "Utils/Stream.h"
#include "Utils/Utils.h"

using namespace Silent::Utils;

namespace Silent::Assets
{
    /** @brief TMD mesh metadata. */
    struct MeshMetadata
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
        constexpr int FIXP_FLAG = 1 << 0;

        // Read file.
        auto stream = Stream(filename, true, false);
        if (!stream.IsOpen())
        {
            throw std::runtime_error(Fmt("Failed to open TMD `{}`.", filename.string()));
        }

        // Read version (unused).
        uint32 ver = stream.ReadUint32();

        // Read flags.
        uint32 flags = stream.ReadUint32();

        // Read mesh count.
        uint16 meshCount = stream.ReadUint16();

        // Read mesh metadatas.
        auto metadatas = std::vector<MeshMetadata>(meshCount);
        for (auto& metadata : metadatas)
        {
            // Read vertex data.
            metadata.VertexOffset = stream.ReadUint32();
            metadata.VertexCount  = stream.ReadUint32();

            // Read normal data.
            metadata.NormalOffset = stream.ReadUint32();
            metadata.NormalCount  = stream.ReadUint32();

            // Read primitive data.
            metadata.PrimitiveOffset = stream.ReadUint32();
            metadata.PrimitiveCount  = stream.ReadUint32();

            // Read scale.
            metadata.Scale = stream.ReadUint32();

            if (!(flags & FIXP_FLAG))
            {
                // Something.
            }
        }

        // Create asset.
        auto asset = TmdAsset
        {
            .Meshes = std::vector<TmdAsset::Mesh>(meshCount)
        };

        // Read meshes.
        for (int i = 0; i < meshCount; i++)
        {
            auto&       mesh     = asset.Meshes[i];
            const auto& metadata = metadatas[i];

            // Read vertices.
            mesh.Vertices.reserve(metadata.VertexCount);
            for (int j = 0; j < metadata.VertexCount; j++)
            {
                // Read components.
                int16 x   = stream.ReadInt16();
                int16 y   = stream.ReadInt16();
                int16 z   = stream.ReadInt16();
                int16 pad = stream.ReadInt16();

                // Collect vertex.
                mesh.Vertices.push_back(Vector3(x, y, z));
            }

            // Read normals.
            mesh.Normals.reserve(metadata.NormalCount);
            for (int j = 0; j < metadata.NormalCount; j++)
            {
                // Read components.
                int16 x   = stream.ReadInt16();
                int16 y   = stream.ReadInt16();
                int16 z   = stream.ReadInt16();
                int16 pad = stream.ReadInt16();

                // Collect normal.
                auto normal = Vector3::Normalize((Vector3(x, y, z) / 4096.0f));
                mesh.Normals.push_back(normal);
            }

            // Read primitives.
            mesh.Triangles.reserve(metadata.PrimitiveCount);
            for (int j = 0; j < metadata.PrimitiveCount; j++)
            {
                // Read attributes.
                int8 olen  = stream.ReadInt8();
                int8 ilen  = stream.ReadInt8();
                int8 flags = stream.ReadInt8();
                int8 mode  = stream.ReadInt8();

                //????

                // Read vertex indices.
                /*uint16 vertIdx0 = stream.ReadUnt16();
                uint16 vertIdx1 = stream.ReadUnt16();
                uint16 vertIdx2 = stream.ReadUnt16();

                // Read normal indices.
                uint16 normalIdx0 = stream.ReadUnt16();
                uint16 normalIdx1 = stream.ReadUnt16();
                uint16 normalIdx2 = stream.ReadUnt16();

                // Collect triangle;
                mesh.Triangles.push_back(TmdAsset::Triangle
                {
                    .Vertices = { vertIdx0, vertIdx1, vertIdx2 },
                    .Normals  = { normalIdx0, normalIdx1, normalIdx2 }
                });*/
            }
        }

        return std::make_shared<TmdAsset>(std::move(asset));
    }
}
