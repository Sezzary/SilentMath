#pragma once

namespace Silent::Renderer
{
    struct Triangle
    {
    private:
        static constexpr uint VERTEX_COUNT = 3;

    public:
        std::array<Vector3, VERTEX_COUNT> Vertices = {};
        Color                             Col      = Color::Black;
    };
}
