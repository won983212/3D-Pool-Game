#pragma once
#include <string>
#include <unordered_map>
#include "../gfx/gfx.h"
#include "../gfx/texture.h"
#include "../util/util.h"

#define MESH_TEXTURE_TYPE_SIZE 4
#define MESH_MATERIAL_TYPE_SIZE 4
#define DEFAULT_AO 0.3f

namespace model
{
	const std::string textureUniformNames[MESH_TEXTURE_TYPE_SIZE] =
	{
		"texture_albedo",
		"texture_metallic",
		"texture_roughness",
		"texture_normal"
	};

	const int texturePBRIndexes[MESH_TEXTURE_TYPE_SIZE] =
	{
		PBR_TEXTURE_INDEX_ALBEDO,
		PBR_TEXTURE_INDEX_METALLIC,
		PBR_TEXTURE_INDEX_ROUGHNESS,
		PBR_TEXTURE_INDEX_NORMAL
	};

	enum class TextureType
	{
		ALBEDO, METALLIC, ROUGHNESS, NORMAL
	};

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 texCoord;
	};

	struct Material
	{
		glm::vec4 albedo;
		float metallic;
		float roughness;
		float ao;
		int texIndex[MESH_TEXTURE_TYPE_SIZE];
	};

	void bindMaterial(const struct Material* materials);
}