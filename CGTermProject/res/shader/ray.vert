#version 420
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec4 aColor;

out vec4 color;

layout (std140, binding = 3) uniform ViewMat {
	mat4 view;
	mat4 projection;
};


void main()
{
	color = aColor;
	gl_Position = projection * view * vec4(aPosition, 1.0);
}