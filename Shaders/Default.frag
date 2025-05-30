#version 460 core

out vec4 FragColor;

// Inputs color from vertex shader.
in vec3 color;

in vec2 texCoord;

uniform sampler2D tex0;

void main()
{
    FragColor = texture(tex0, texCoord);
}
