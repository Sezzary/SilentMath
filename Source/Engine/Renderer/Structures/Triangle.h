#pragma once

namespace Silent::Renderer
{
    struct Triangle
    {
    private:
        static constexpr uint VERTEX_COUNT = 3;

    public:
        Vector3 Vertices[VERTEX_COUNT] = {};
        Vector4 Color                  = {};
    };
}
