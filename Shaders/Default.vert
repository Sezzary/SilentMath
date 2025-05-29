#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

// Outputs color for fragment shader.
out vec3 color;

uniform float scale;

void main()
{
    gl_Position = vec4(aPos.x + (aPos.x * scale),
                       aPos.y + (aPos.z * scale),
                       aPos.z * (aPos.z * scale), 1.0);

    // Assigns colors from vertex data to `color`.
    color = aColor;
}
