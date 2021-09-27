#include "texture.h"
#include "model.h"
#include "../util/util.h"

using namespace model;
using namespace std;

const string textureUniformNames[MESH_TEXTURE_TYPE_SIZE] =
{
	"texture_diffuse",
	"texture_specular",
	"texture_normal",
	"texture_height"
};


Mesh::Mesh(vector<Vertex> verties, vector<unsigned int> indices, vector<Texture> textures)
	: ebo(GL_ELEMENT_ARRAY_BUFFER)
{
	this->verties = verties;
	this->indices = indices;
	this->textures = textures;
	setupMesh();
}

void Mesh::Draw(ShaderProgram& shader)
{
	unsigned int id[MESH_TEXTURE_TYPE_SIZE] = { 0, };
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		TextureType type = textures[i].type;
		string name = textureUniformNames[(int)type] + to_string(id[(int)type]++);

		glActiveTexture(GL_TEXTURE0 + i);
		shader.setUniform(name.c_str(), (int)i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	vao.bind();
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	VAO::unbind();

	glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
	vao.create();
	vbo.create();
	ebo.create();

	vao.bind();
	vbo.buffer(verties.size() * sizeof(Vertex), &verties[0]);
	ebo.buffer(indices.size() * sizeof(unsigned int), &indices[0]);

	vao.attr(0, 3, GL_FLOAT, sizeof(Vertex), 0);
	vao.attr(1, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, normal));
	vao.attr(2, 2, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, texCoord));

	VAO::unbind();
}

void Model::Draw(ShaderProgram& shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].Draw(shader);
}

void Model::loadModel(string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "Error: Can't load model: " << importer.GetErrorString() << endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

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
			vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = mesh->mTextureCoords[0][i].y;
		}
		else
			vertex.texCoord = glm::vec2(0.0f);

		vertices.push_back(vertex);
	}

	// indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
			indices.push_back(mesh->mFaces[i].mIndices[j]);
	}

	// materials
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// diffuse
		vector<model::Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		// specular
		vector<model::Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		// normal
		vector<model::Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, TextureType::NORMAL);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		// height
		vector<model::Texture> heightMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, TextureType::HEIGHT);
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

vector<model::Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType textureType)
{
	vector<model::Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		std::string str_s(str.C_Str());
		if (loaded_textures.find(str_s) != loaded_textures.end())
		{
			textures.push_back(loaded_textures[str_s]);
		}
		else
		{
			commoncg::Texture texture;
			texture.loadImage((directory + '/' + str_s).c_str());

			model::Texture tex;
			tex.id = texture.getTextureID();
			tex.type = textureType;
			tex.path = str.C_Str();
			textures.push_back(tex);
			loaded_textures.insert(std::make_pair(tex.path, tex));
		}
	}
	return textures;
}