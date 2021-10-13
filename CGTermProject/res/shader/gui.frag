#version 330
in vec4 color;
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D texture_diffuse;

void main()
{
	fragColor = color * texture(texture_diffuse, texCoord);
}