#version 460 core

in vec3 vertColor;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D tex0;
uniform sampler2D tex1;

void main()
{
    //FragColor = vertColor;
    //FragColor = texture(tex, texCoord) * vec4(vertColor, 1.0);
    FragColor = mix(texture(tex0, texCoord), texture(tex1, texCoord), 0.5);
}
