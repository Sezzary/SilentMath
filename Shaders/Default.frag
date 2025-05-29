#version 460 core

out vec4 FragColor;

// Inputs color from vertex shader.
in vec3 color;

void main()
{
    FragColor = vec4(color, 1.0f);
}
