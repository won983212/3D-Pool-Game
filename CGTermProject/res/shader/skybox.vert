#version 420
layout (location = 0) in vec3 aPos;

out vec3 texCoords;

layout (std140, binding = 3) uniform ViewMat {
	mat4 view;
	mat4 projection;
} view;

void main()
{
	texCoords = aPos;
	vec4 pos = view.projection * view.view * vec4(aPos, 1.0f);
	gl_Position = pos.xyww;
}