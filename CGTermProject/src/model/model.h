#pragma once
#include <string>
#include <unordered_map>
#include "../gfx/gfx.h"
#include "../util/util.h"

constexpr int MeshTextureTypeSize = 4;
constexpr int MeshMaterialTypeSize = 4;
constexpr float DefaultAo = 0.3f;

namespace model
{
	const std::string TextureUniformNames[MeshTextureTypeSize] =
	{
		"texture_albedo",
		"texture_metallic",
		"texture_roughness",
		"texture_normal"
	};

	constexpr int TexturePbrIndexes[MeshTextureTypeSize] =
	{
		PBR_TEXTURE_INDEX_ALBEDO,
		PBR_TEXTURE_INDEX_METALLIC,
		PBR_TEXTURE_INDEX_ROUGHNESS,
		PBR_TEXTURE_INDEX_NORMAL
	};

	enum class TextureType
	{
		ALBEDO,
		METALLIC,
		ROUGHNESS,
		NORMAL
	};

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 tex_coord;
	};

	struct Material
	{
		glm::vec4 albedo;
		float metallic;
		float roughness;
		float ao;
		int tex_index[MeshTextureTypeSize];
	};

	void BindMaterial(const struct Material* materials);
}
