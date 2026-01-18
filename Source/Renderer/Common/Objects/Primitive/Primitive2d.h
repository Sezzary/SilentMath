#pragma once

namespace Silent::Renderer
{
    /** @brief 2D screen primitive representing a quad or triangle. */
    struct Primitive2d
    {
        std::vector<Vertex2d> Vertices = {};
        int                   Depth    = 0;
        BlendMode             BlendMd  = BlendMode::Alpha;
    };
}
