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
		void draw(commoncg::ShaderProgram& shader);
	};

	class AssetModel
	{
	public:
		AssetModel() {};
		~AssetModel();
		void draw(commoncg::ShaderProgram& shader);
		void loadModel(std::string path);
	public:
		std::vector<Mesh*> meshes;
		std::vector<struct Material*> materials;
		std::vector<ModelTexture*> textures;
		std::string directory;
	};
}