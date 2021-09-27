#version 330

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 normal;
out vec3 fragPos;
out vec2 texCoord;

uniform mat4 m, v, p;

void main()
{
	gl_Position = p * v * m * vec4(aPosition, 1.0f);
	fragPos = vec3(m * vec4(aPosition, 1.0));
	normal = aNormal;
	texCoord = aTexCoord;
}