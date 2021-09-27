#version 330
out vec4 FragColor;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct DirectionalLight {
	vec3 direction;
	Light light;
};

struct PointLight {
	vec3 position;
	float constant;
	float linear;
	float quadratic;
	Light light;
};

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

uniform vec3 viewPos;
uniform Material material;

uniform DirectionalLight dirLight;
uniform PointLight pointLights[4];



vec3 CalculatePhongLighting(Light light, vec3 lightVec, vec3 normal, vec3 viewVec){
	// ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoord));

	// diffuse
	float diffuseScale = max(dot(normal, lightVec), 0);
	vec3 diffuse = light.diffuse * diffuseScale * vec3(texture(material.diffuse, texCoord));

	// specular
	vec3 reflectVec = reflect(-lightVec, normal);
	float specularScale = pow(max(dot(viewVec, reflectVec), 0.0), material.shininess);
	vec3 specular = light.specular * specularScale * vec3(texture(material.specular, texCoord));

	return (ambient + diffuse + specular);
}

vec3 CalculateDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewVec)
{
	vec3 lightVec = normalize(-light.direction);
	return CalculatePhongLighting(light.light, normal, lightVec, viewVec);
}

vec3 CalculatePointLight(PointLight light, vec3 normal, vec3 viewVec)
{
	vec3 lightVec = normalize(light.position - fragPos);

	// attenuation
	float dist = length(light.position - fragPos);
	float invAttenuation = light.constant + light.linear * dist + light.quadratic * dist * dist;

	return CalculatePhongLighting(light.light, normal, lightVec, viewVec) / invAttenuation;
}

void main()
{
	vec3 norm = normalize(normal);
	vec3 viewVec = normalize(viewPos - fragPos);

	// Directional Light
	vec3 result = CalculateDirectionalLight(dirLight, norm, viewVec);

	// Point light
	/*for (int i = 0; i < 4; i++)
		result += CalculatePointLight(pointLights[i], norm, viewVec);*/

	FragColor = vec4(result, 1.0f);
}