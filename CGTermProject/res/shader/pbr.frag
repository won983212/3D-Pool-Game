#version 420
const int LIGHT_SIZE = 3;
const float PI = 3.14159265359;
const float MAX_REFLECTION_LOD = 7.0;

out vec4 fragColor;

in VS_OUT
{
	vec3 worldPos;
	vec3 normal;
	vec2 texCoord;
};

layout (std140, binding = 1) uniform Material 
{
	vec4 albedo;
	float metallic;
	float roughness;
	float ao;
	int texIndexAlbedo;
	int texIndexMetallic;
	int texIndexRoughness;
	int texIndexNormal;
} material;

struct LightData 
{
	vec4 position;
	vec4 color;
};

layout (std140, binding = 2) uniform Light 
{
	LightData lights[LIGHT_SIZE];
};

uniform vec3 camPos;

// textures
uniform sampler2D texture_albedo;
uniform sampler2D texture_metallic;
uniform sampler2D texture_roughness;
uniform sampler2D texture_normal;

// env maps
uniform samplerCube irradianceMap;
uniform samplerCube specularMap;
uniform sampler2D brdfMap;


// calculate normal from normal map.
vec3 calculateNormal()
{
    vec3 N = normalize(normal);
	if(material.texIndexNormal == -1)
		return N;

    vec3 tangentNorm = texture(texture_normal, texCoord).xyz * 2.0 - 1.0;
    vec3 T  = normalize(dFdx(worldPos) * dFdx(texCoord).t - dFdy(worldPos) * dFdy(texCoord).t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

	// normal space -> tangent space (actual normal direction)
    return normalize(TBN * tangentNorm);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = pow(roughness, 4);
	float NdotH = max(dot(N, H), 0.0);
	float denom = (NdotH * NdotH * (a - 1.0) + 1.0);
	return a / (denom * denom * PI);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;
	return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(float NdotV, float NdotL, float roughness)
{
	return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
	vec3 albedo =		material.texIndexAlbedo != -1 ?		pow(texture(texture_albedo, texCoord).rgb, vec3(2.2)) :		vec3(material.albedo);
	float metallic =	material.texIndexMetallic != -1 ?	texture(texture_metallic, texCoord).r :						material.metallic;
	float roughness =	material.texIndexRoughness != -1 ?	texture(texture_roughness, texCoord).r :					material.roughness;
	float ao =			material.ao;

	vec3 N = calculateNormal();
	vec3 V = normalize(camPos - worldPos);
	vec3 R = reflect(-V, N);
	float NdotV = max(dot(N, V), 0.0);
	vec3 F0 = mix(vec3(0.04), albedo, metallic);

	// calculate light affacts
	vec3 Lo = vec3(0.0);
	for(int i = 0; i < LIGHT_SIZE; i++)
	{
		vec3 L = normalize(vec3(lights[i].position) - worldPos);
		vec3 H = normalize(V + L);
		float NdotL = max(dot(N, L), 0.0);

		// radiance
		float dist = length(vec3(lights[i].position) - worldPos);
		vec3 radiance = vec3(lights[i].color) / (dist * dist);

		// specular (use brdf)
		float D = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(NdotV, NdotL, roughness);
		vec3 F = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0, 0);
		vec3 specular = (D * G * F) / (4 * NdotV * NdotL + 0.0001);

		// Ks = F, Kd = 1 - F
		vec3 kD = (1.0 - F) * (1.0 - metallic);
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
	}

	// calculate skybox environment lighting (ambient)
	vec3 kS = FresnelSchlick(NdotV, F0, roughness);
	vec3 kD = (1.0 - kS) * (1.0 - metallic);

	// diffuse
	vec3 diffuse = texture(irradianceMap, N).rgb * albedo;

	// specular (use brdf)
	vec3 specularColor = textureLod(specularMap, R, roughness * MAX_REFLECTION_LOD).rgb;
	vec2 brdf = texture(brdfMap, vec2(NdotV, roughness)).rb;
	vec3 specular = specularColor * (kS * brdf.x + brdf.y);
	
	// total ambient
	vec3 ambient = (kD * diffuse + specular) * ao;

	// skybox env light + direct light sources
	vec3 color = ambient + Lo;
	color = color / (color + vec3(1.0)); // HDR
	color = pow(color, vec3(1.0/2.2)); // gamma correction

	// result
	fragColor = vec4(color, 1.0);
}