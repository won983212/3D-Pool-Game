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

namespace model
{
	enum class TextureType
	{
		DIFFUSE, SPECULAR, NORMAL, HEIGHT
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
		std::string path;
	};

	class Mesh
	{
	public:
		std::vector<Vertex> verties;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
	private:
		commoncg::VAO vao;
		commoncg::VBO vbo, ebo;
	public:
		Mesh(std::vector<Vertex> verties, std::vector<unsigned int> indices, std::vector<Texture> textures);
		void Draw(commoncg::ShaderProgram& shader);
	private:
		void setupMesh();
	};

	class Model
	{
	public:
		void Draw(commoncg::ShaderProgram& shader);
		void loadModel(std::string path);
	private:
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<model::Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType textureType);
	private:
		std::unordered_map<std::string, model::Texture> loaded_textures;
		std::vector<Mesh> meshes;
		std::string directory;
	};
}