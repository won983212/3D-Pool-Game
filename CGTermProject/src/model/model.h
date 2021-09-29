#pragma once
#include <string>
#include <unordered_map>
#include "../gfx/gfx.h"
#include "../gfx/texture.h"

#define MESH_TEXTURE_TYPE_SIZE 4
#define MESH_MATERIAL_TYPE_SIZE 4

namespace model
{
	const std::string textureUniformNames[MESH_TEXTURE_TYPE_SIZE] =
	{
		"texture_diffuse",
		"texture_specular",
		"texture_normal",
		"texture_height"
	};

	enum class TextureType
	{
		DIFFUSE, SPECULAR, NORMAL, HEIGHT
	};

	enum class MaterialColorType
	{
		AMBIENT, DIFFUSE, SPECULAR, EMISSIVE
	};

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
	};

	struct Material
	{
		float diffuse[4];
		float ambient[4];
		float specular[4];
		float emissive[4];
		int texIndex[4];
		float shininess;
	};

	void bindMaterial(const struct Material* materials);
}