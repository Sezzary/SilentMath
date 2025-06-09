#version 460 core

// Layout
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

// Outputs
out vec3 vertColor;
out vec2 texCoord;

// Uniforms
uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

void main()
{
    gl_Position = (projMat * viewMat * modelMat) * vec4(aPos, 1.0);
    vertColor   = aColor;
    texCoord    = aTexCoord;
}
