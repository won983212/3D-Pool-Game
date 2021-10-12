#version 330
layout (location = 0) in vec3 aPosition;

out vec3 worldPos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	worldPos = aPosition;
	gl_Position = projection * view * vec4(worldPos, 1.0);
}