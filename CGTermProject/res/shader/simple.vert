#version 330

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 fragPos;
out vec3 normal;
out vec2 texCoord;

uniform mat4 m, v, p;

void main()
{
	gl_Position = p * v * m * vec4(aPosition.x, aPosition.y, aPosition.z, 1.0f);
	fragPos = aPosition;
	normal = aNormal;
	texCoord = aTexCoord;
}