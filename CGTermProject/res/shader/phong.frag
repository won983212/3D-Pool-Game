#version 330

out vec4 FragColor;

in vec3 pos;
in vec3 normal;
in vec2 texCoord;

uniform vec3 camPos;

void main()
{
	// 프로그램을 간소화하기 위해 쉐이더에 직접 color / material / light direction 대입
	vec3 color = vec3(1.0, 0.0, 0.0);
	vec3 lightDir = normalize(vec3(1.0f, 1.0f, 1.0f));
	vec3 norm = normalize(normal);
	vec3 viewVec = normalize(camPos - pos);

	vec3 ambient = vec3(0.1) * vec3(color);
	vec3 diffuse = vec3(max(dot(lightDir, norm), 0.0f) * 0.5) * vec3(color);
	
	// specular
	vec3 reflectVec = reflect(-lightDir, normal);
	float specularScale = pow(max(dot(viewVec, reflectVec), 0.0f), 10);
	vec3 specular = vec3(0.08) * specularScale;

	FragColor = vec4(ambient + diffuse + specular, 1.0f);
}