#version 330
in vec4 color;
in vec2 texCoord;
out vec4 fragColor;

uniform bool useTexture;
uniform bool useFont;
uniform sampler2D texture_diffuse;

void main()
{
	if (useFont)
	{
		fragColor = vec4(color.rgb, color.a * texture(texture_diffuse, texCoord).r);
	}
	else
	{
		fragColor = useTexture ? color * texture(texture_diffuse, texCoord) : color;
	}
}