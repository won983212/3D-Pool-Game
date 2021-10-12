#version 330
out vec4 fragColor;
in vec3 worldPos;

uniform samplerCube skyboxMap;

void main()
{
	vec3 color = texture(skyboxMap, worldPos).rgb;

	// HDR, gamma correction
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2));

	fragColor = vec4(color, 1.0);
}