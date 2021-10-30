#include "assetmodel.h"
#include <iostream>
#include "../gfx/shader.h"
#include "../gfx/texture.h"
#include "assimp/postprocess.h"
#include "assimp/pbrmaterial.h"


using namespace model;
using namespace commoncg;
using namespace std;


static string GetTexPathAssimp(const aiMaterial* material, aiTextureType type, unsigned index)
{
	if (material->GetTextureCount(type) > 0)
	{
		aiString ai_filename;
		if (material->GetTexture(type, index, &ai_filename) == AI_SUCCESS)
			return ai_filename.C_Str();
	}
	return "";
}

static glm::vec4 GetVecAssimp(const aiMaterial* material, glm::vec4 default_value, const char* key, unsigned type, unsigned index)
{
	aiColor4D ai_color;
	if (material->Get(key, type, index, ai_color) == AI_SUCCESS)
		return glm::vec4(ai_color.r, ai_color.g, ai_color.b, ai_color.a);
	return default_value;
}

static float GetFloatAssimp(const aiMaterial* material, float default_value, const char* key, unsigned type, unsigned index)
{
	float data;
	if (material->Get(key, type, index, data) == AI_SUCCESS)
		return data;
	return default_value;
}

Mesh::Mesh(const AssetModel* parent, const aiMesh* mesh)
	: ebo_(GL_ELEMENT_ARRAY_BUFFER), material_index_(-1), indice_size_(0), parent_(parent)
{
	vector<Vertex> vertices;
	vector<unsigned int> indices;

	// vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		// position
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		// normal
		if (mesh->HasNormals())
		{
			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;
		}

		// texture
		if (mesh->HasTextureCoords(0))
		{
			vertex.tex_coord.x = mesh->mTextureCoords[0][i].x;
			vertex.tex_coord.y = mesh->mTextureCoords[0][i].y;
		}
		else
			vertex.tex_coord = glm::vec2(0.0f);

		vertices.push_back(vertex);
	}

	// indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
			indices.push_back(mesh->mFaces[i].mIndices[j]);
	}
	indice_size_ = indices.size();

	// materials
	material_index_ = mesh->mMaterialIndex;

	// allocate at VAO
	vao_.Create();
	vbo_.Create();
	ebo_.Create();

	vao_.Use();
	vbo_.Buffer(vertices.size() * sizeof(Vertex), &vertices[0]);
	ebo_.Buffer(indices.size() * sizeof(unsigned int), &indices[0]);

	vao_.Attrib(0, 3, GL_FLOAT, sizeof(Vertex), 0);
	vao_.Attrib(1, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, normal));
	vao_.Attrib(2, 2, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, tex_coord));

	VAO::Unbind();
}

void Mesh::Draw() const
{
	const Material* u_mat = parent_->materials_[material_index_];
	for (unsigned int i = 0; i < MeshTextureTypeSize; i++)
	{
		glActiveTexture(GL_TEXTURE0 + TexturePbrIndexes[i]);
		Texture::Unbind();

		// ignore not loaded texture.
		if (u_mat->tex_index[i] == -1)
			continue;
		
		// get texture info
		ModelTexture* texture = parent_->textures_[u_mat->tex_index[i]];
		string name = TextureUniformNames[static_cast<int>(texture->type)];

		// Bind texture
		ShaderProgram::GetContextShader()->SetUniform(name.c_str(), TexturePbrIndexes[i]);
		texture->texture->Bind();
	}

	// Bind material to shader
	if (material_index_ >= 0)
		BindMaterial(parent_->materials_[material_index_]);

	vao_.Use();
	glDrawElements(GL_TRIANGLES, indice_size_, GL_UNSIGNED_INT, nullptr);
	VAO::Unbind();
}

void AssetModel::Draw()
{
	for (const auto& mesh : meshes_)
		mesh->Draw();

	// all texture Unbind
	for (const int tex_idx : TexturePbrIndexes)
	{
		glActiveTexture(GL_TEXTURE0 + tex_idx);
		Texture::Unbind();
	}

	glActiveTexture(GL_TEXTURE0);
}

void AssetModel::LoadTexture(const aiScene* scene, const string& tex_path, Material* u_mat, int id)
{
	// load texture
	const auto tex = new ModelTexture();
	tex->texture = Texture::CacheImage((directory_ + '/' + tex_path).c_str(), GL_REPEAT, true);
	tex->type = static_cast<TextureType>(id);

	// set textureIndex
	u_mat->tex_index[id] = textures_.size();
	textures_.push_back(tex);
}

void AssetModel::LoadModel(const string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "Error: Can't load model: " << importer.GetErrorString() << endl;
		return;
	}

	// resource path
	directory_ = path.substr(0, path.find_last_of('/'));

	// load all materials
	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		const aiMaterial* material = scene->mMaterials[i];
		auto u_mat = new Material();

		for (int& j : u_mat->tex_index)
			j = -1;

		// load textures
		string albedo_tex_path = GetTexPathAssimp(material, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE);
		if (!albedo_tex_path.empty())
			LoadTexture(scene, albedo_tex_path, u_mat, 0);

		string mr_tex_path = GetTexPathAssimp(material, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE);
		if (!mr_tex_path.empty())
		{
			const int dash_idx = mr_tex_path.find('-');
			if (dash_idx == string::npos)
			{
				LoadTexture(scene, mr_tex_path, u_mat, 1);
			}
			else
			{
				LoadTexture(scene, mr_tex_path.substr(0, dash_idx), u_mat, 1);
				if (mr_tex_path.size() > dash_idx + 1)
					LoadTexture(scene, mr_tex_path.substr(dash_idx + 1), u_mat, 2);
			}
		}

		string normal_tex_path = GetTexPathAssimp(material, aiTextureType_NORMALS, 0);
		if (!normal_tex_path.empty())
			LoadTexture(scene, normal_tex_path, u_mat, 3);

		// uniform block material setup
		u_mat->albedo = GetVecAssimp(material, glm::vec4(1.0f), AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_FACTOR);
		u_mat->metallic = GetFloatAssimp(material, 1.0f, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR);
		u_mat->roughness = GetFloatAssimp(material, 1.0f, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR);
		u_mat->ao = DefaultAo;

		// add to material dictionary
		materials_.push_back(u_mat);
	}

	// load all meshes
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		meshes_.push_back(new Mesh(this, mesh));
	}
}

AssetModel::~AssetModel()
{
	for (const auto& mesh : meshes_)
		delete mesh;
	for (const auto& material : materials_)
		delete material;
	for (const auto& texture : textures_)
		delete texture;
}
