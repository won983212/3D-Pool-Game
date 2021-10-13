#version 330
layout (location = 0) in vec4 aPosTex;
layout (location = 1) in vec4 aColor;

out vec4 color;
out vec2 texCoord;

uniform mat4 projection;

void main()
{
	color = aColor;
	texCoord = aPosTex.zw;
	gl_Position = projection * vec4(aPosTex.xy, 0.0, 1.0);
}