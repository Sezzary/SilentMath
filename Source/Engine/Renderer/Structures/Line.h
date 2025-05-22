#pragma once

namespace Silent::Renderer
{
    struct Line
    {
        Vector3 From = Vector3::Zero;
        Vector3 To   = Vector3::Zero;
        Color   Col  = Color::Black;
    };
}
