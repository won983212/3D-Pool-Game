#version 420
layout (location = 0) in vec3 aPosition;

layout (std140, binding = 3) uniform ViewMat {
	mat4 view;
	mat4 projection;
};

void main()
{
	gl_Position = projection * view * vec4(aPosition, 1.0);
}