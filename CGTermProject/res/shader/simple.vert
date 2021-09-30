#version 420

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (std140, binding = 3) uniform ViewMat {
	mat4 view;
	mat4 projection;
} view;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;

uniform mat4 model;

void main()
{
	gl_Position = view.projection * view.view * model * vec4(aPosition, 1.0f);
	fragPos = mat3(model) * aPosition;
	normal = normalize(mat3(model) * aNormal);
	texCoord = aTexCoord;
}