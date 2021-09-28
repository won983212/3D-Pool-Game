#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "vao.h"
#include "vbo.h"
#include "shader.h"

#define MESH_TEXTURE_TYPE_SIZE 4
#define MESH_MATERIAL_TYPE_SIZE 4


namespace model
{
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

	struct Texture
	{
		unsigned int id;
		TextureType type;
	};

	struct Material
	{
		float diffuse[4];
		float ambient[4];
		float specular[4];
		float emissive[4];
		float shininess;
		int texCount[MESH_MATERIAL_TYPE_SIZE];
	};

	class Mesh
	{
	public:
		std::vector<Texture> textures;
	private:
		commoncg::VAO vao;
		commoncg::VBO vbo, ebo, u_material;
		struct Material uMat;
		int indiceSize;
	public:
		Mesh(std::string directory, aiMesh* mesh, const aiScene* scene);
		std::vector<model::Texture> loadMaterialTextures(std::string directory, aiMaterial* mat, aiTextureType type, TextureType textureType);
		void draw(commoncg::ShaderProgram& shader);
	private:
		void bindMaterial() const;
	};

	class Model
	{
	public:
		void draw(commoncg::ShaderProgram& shader);
		void loadModel(std::string path);
	private:
		std::vector<Mesh> meshes;
		std::string directory;
	};
}