#include "Framework.h"
#include "Renderer/Common/Resources/Scene/Triangle3d.h"

#include "Renderer/Common/Enums.h"
#include "Renderer/Common/Utils.h"

namespace Silent::Renderer
{
    Triangle3d Triangle3d::CreateTriangle3d(const Vector3& vert0, const Vector3& vert1, const Vector3& vert2,
                                            const Color& color,
                                            BlendMode blendMode)
    {
        auto normal = GetNormal(vert0, vert1, vert2);

        return Triangle3d
        {
            .Vertices =
            {
                Triangle3dVertex
                {
                    .Position = vert0,
                    .Normal   = normal,
                    .Col      = color
                },
                Triangle3dVertex
                {
                    .Position = vert1,
                    .Normal   = normal,
                    .Col      = color
                },
                Triangle3dVertex
                {
                    .Position = vert2,
                    .Normal   = normal,
                    .Col      = color
                },
            },
            .BlendMd = blendMode
        };
    }
}
