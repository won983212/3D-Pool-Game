#pragma once

#include <vector>
#include <string>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "model.h"
#include "../gfx/vao.h"
#include "../gfx/vbo.h"
#include "../gfx/texture.h"


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
		unsigned int indice_size_;
		const AssetModel* parent_;

	public:
		Mesh(const AssetModel* parent, const aiMesh* mesh);
		void Draw() const;
	};

	class AssetModel
	{
	public:
		AssetModel() = default;
		~AssetModel();
		void Draw();
		void LoadModel(const std::string& path);

	private:
		void LoadTexture(const aiScene* scene, const std::string& tex_path, Material* u_mat, int id);

	public:
		std::vector<Mesh*> meshes_;
		std::vector<struct Material*> materials_;
		std::vector<ModelTexture*> textures_;
		std::string directory_;
	};
}
