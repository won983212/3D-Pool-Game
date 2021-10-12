#version 330
out vec4 FragColor;
in vec3 worldPos;

uniform float lod = 0.0;
uniform bool flipY = false;
uniform sampler2D equirectangularMap;

const vec2 invAngleRange = vec2(0.159155, 0.318310); // 1/2pi, 1/pi
void main()
{
	vec3 dir = normalize(worldPos);
	vec2 uv = vec2(atan(dir.z, dir.x), asin(dir.y)); // -2pi ~ 2pi, -pi ~ pi

	// adjust uv range
	uv *= invAngleRange; // to -0.5 ~ 0.5
	uv += 0.5; // to 0.0 ~ 1.0
	uv.y = flipY ? (1 - uv.y) : uv.y;

	vec4 color = textureLod(equirectangularMap, uv, lod);
	FragColor = vec4(color.rgb, 1.0f);
}