#version 420

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
	LightData lights[3];
};

out vec4 FragColor;

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

uniform vec3 camPos;
uniform sampler2D texture_albedo;

void main()
{
	vec3 albedo = material.texIndexAlbedo != -1 ? texture(texture_albedo, texCoord).rgb : vec3(material.albedo);
	
	vec3 lightDir = normalize(vec3(1.0f, 1.0f, 1.0f));
	vec3 norm = normalize(normal);
	vec3 viewVec = normalize(camPos - fragPos);

	// ambient: 0.2
	vec3 ambient = vec3(lights[0].color) * 0.01;
	
	// diffuse: 0.8
	float diffuseScale = max(dot(lightDir, norm), 0.0f);
	vec3 diffuse = vec3(lights[0].color) * diffuseScale * 0.02;

	// apply texture
	diffuse *= vec3(albedo);
	ambient *= vec3(albedo);
	
	// specular
	vec3 reflectVec = reflect(-lightDir, normal);
	float specularScale = pow(max(dot(viewVec, reflectVec), 0.0f), 10);
	vec3 specular = vec3(0.08) * specularScale;

	FragColor = vec4(ambient + diffuse + specular, 1.0f);
}