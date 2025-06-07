#version 460 core

in vec3 vertColor;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D tex;

void main()
{
    //FragColor = vertColor;
    FragColor = texture(tex, texCoord) * vec4(vertColor, 1.0);
}
