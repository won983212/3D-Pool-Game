#pragma once

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "model.h"
#include "../gfx/vao.h"
#include "../gfx/vbo.h"
#include "../gfx/shader.h"


namespace model
{
	struct ModelTexture
	{
		const commoncg::Texture* texture;
		TextureType type;
	};

	class AssetModel;
	class Mesh
	{
	private:
		commoncg::VAO vao;
		commoncg::VBO vbo, ebo;
		int materialIndex;
		int indiceSize;
		const AssetModel* parent;
	public:
		Mesh(const AssetModel* parent, aiMesh* mesh, const aiScene* scene);
		void draw();
	};

	class AssetModel
	{
	public:
		AssetModel() {};
		~AssetModel();
		void draw();
		void loadModel(std::string path);
	private:
		void loadTexture(const aiScene* scene, std::string texPath, Material* uMat, int id);
	public:
		std::vector<Mesh*> meshes;
		std::vector<struct Material*> materials;
		std::vector<ModelTexture*> textures;
		std::string directory;
	};
}