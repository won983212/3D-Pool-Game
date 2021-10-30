#pragma once

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "model.h"
#include "../gfx/vao.h"
#include "../gfx/vbo.h"


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
		commoncg::VAO vao_;
		commoncg::VBO vbo_, ebo_;
		int material_index_;
		int indice_size_;
		const AssetModel* parent_;

	public:
		Mesh(const AssetModel* parent, aiMesh* mesh);
		void Draw();
	};

	class AssetModel
	{
	public:
		AssetModel()
		{
		};
		~AssetModel();
		void Draw();
		void LoadModel(std::string path);

	private:
		void LoadTexture(const aiScene* scene, const std::string& tex_path, Material* u_mat, int id);

	public:
		std::vector<Mesh*> meshes_;
		std::vector<struct Material*> materials_;
		std::vector<ModelTexture*> textures_;
		std::string directory_;
	};
}
