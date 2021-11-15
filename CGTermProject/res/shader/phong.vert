#version 430

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;

layout (std140, binding = 3) uniform ViewMat {
	mat4 view;
	mat4 projection;
} view;

uniform mat4 model;

void main()
{
	fragPos = vec3(model * vec4(aPosition, 1.0));
	normal = normalize(mat3(model) * aNormal);
	texCoord = aTexCoord;
	gl_Position = view.projection * view.view * vec4(fragPos, 1.0);
}