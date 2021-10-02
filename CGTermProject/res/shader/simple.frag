#version 420

layout (std140, binding = 1) uniform Material {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec4 emissive;
	int texIndexDiffuse;
	int texIndexSpecular;
	int texIndexNormal;
	int texIndexHeight;
	float shininess;
} material;

layout (std140, binding = 2) uniform Light {
	vec4 direction;
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
} light;

out vec4 FragColor;

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

uniform vec3 viewPos;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

void main()
{
	vec3 lightDir = normalize(-vec3(light.direction));
	vec3 norm = normalize(normal);
	vec3 viewVec = normalize(viewPos - fragPos);

	// ambient
	vec3 ambient = vec3(light.ambient);
	
	// diffuse
	float diffuseScale = max(dot(lightDir, norm), 0.0f);
	vec3 diffuse = vec3(light.diffuse) * diffuseScale;

	// apply texture
	if(material.texIndexDiffuse != -1){
		diffuse = diffuse * vec3(texture(texture_diffuse, texCoord));
		ambient = ambient * vec3(texture(texture_diffuse, texCoord));
	} else {
		diffuse = diffuse * vec3(material.diffuse);
		ambient = ambient * vec3(material.ambient);
	}
	
	// specular
	vec3 reflectVec = reflect(-lightDir, norm);
	float specularScale = pow(max(dot(viewVec, reflectVec), 0.0f), max(material.shininess, 1.0f));
	vec3 specular = 0.5f * vec3(light.specular) * specularScale;
	if(material.texIndexSpecular != -1){
		specular *= vec3(material.specular);
	} else {
		specular *= vec3(texture(texture_specular, texCoord));
	}
	
	FragColor = vec4(ambient + diffuse + specular, 1.0f);
}