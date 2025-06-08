#version 460 core

// Layout
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

// Outputs
out vec3 vertColor;
out vec2 texCoord;

// Uniforms
uniform vec3 offset;
uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(aPos + offset, 1.0);
    vertColor   = aColor;
    texCoord    = aTexCoord;
}
