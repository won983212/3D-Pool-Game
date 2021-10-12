#version 430
layout (location = 0) in vec3 aPosition;

layout (std140, binding = 3) uniform ViewMat {
	mat4 view;
	mat4 projection;
};

out vec3 worldPos;

void main()
{
	worldPos = aPosition;
	vec4 pos = projection * mat4(mat3(view)) * vec4(worldPos, 1.0); // eliminate view translation
	gl_Position = pos.xyww;
}