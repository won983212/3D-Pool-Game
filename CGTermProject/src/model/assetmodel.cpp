#include <iostream>
#include "../gfx/texture.h"
#include "assetmodel.h"

using namespace model;
using namespace commoncg;
using namespace std;

const aiTextureType textureTypes[MESH_TEXTURE_TYPE_SIZE] =
{
	aiTextureType_DIFFUSE,
	aiTextureType_SPECULAR,
	aiTextureType_NORMALS,
	aiTextureType_HEIGHT
};


static void fillMaterialColor(float* dst, MaterialColorType colorType, const aiMaterial* mtl, float def_r, float def_g, float def_b, float def_a)
{
	aiColor4D aiColor;
	aiReturn aiRet;
	float color[] = { def_r, def_g, def_b, def_a };

	switch (colorType)
	{
	case MaterialColorType::AMBIENT:
		aiRet = aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &aiColor);
		break;
	case MaterialColorType::DIFFUSE:
		aiRet = aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &aiColor);
		break;
	case MaterialColorType::SPECULAR:
		aiRet = aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &aiColor);
		break;
	case MaterialColorType::EMISSIVE:
		aiRet = aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &aiColor);
		break;
	default:
		cout << "Error: Unknown color type: " << (int)colorType << endl;
		return;
	}

	if (AI_SUCCESS == aiRet)
	{
		color[0] = aiColor.r;
		color[1] = aiColor.g;
		color[2] = aiColor.b;
		color[3] = aiColor.a;
	}

	memcpy(dst, color, sizeof(color));
}

Mesh::Mesh(const AssetModel* parent, aiMesh* mesh, const aiScene* scene)
	: ebo(GL_ELEMENT_ARRAY_BUFFER), parent(parent), indiceSize(0), materialIndex(-1)
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
	indiceSize = indices.size();

	// materials
	if (mesh->mMaterialIndex >= 0)
		materialIndex = mesh->mMaterialIndex;

	// allocate at VAO
	vao.create();
	vbo.create();
	ebo.create();

	vao.bind();
	vbo.buffer(vertices.size() * sizeof(Vertex), &vertices[0]);
	ebo.buffer(indices.size() * sizeof(unsigned int), &indices[0]);

	vao.attr(0, 3, GL_FLOAT, sizeof(Vertex), 0);
	vao.attr(1, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, normal));
	vao.attr(2, 2, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, texCoord));

	VAO::unbind();
}

void Mesh::draw(ShaderProgram& shader)
{
	Material* uMat = parent->materials[materialIndex];
	for (unsigned int i = 0; i < MESH_TEXTURE_TYPE_SIZE; i++)
	{
		// ignore not loaded texture.
		if (uMat->texIndex[i] == -1)
			continue;

		// get texture info
		ModelTexture* texture = parent->textures[uMat->texIndex[i]];
		string name = textureUniformNames[(int)texture->type];

		// bind texture
		shader.setUniform(name.c_str(), (int)i);
		glActiveTexture(GL_TEXTURE0 + i);
		texture->texture->bind();
	}

	// bind material to shader
	if (materialIndex >= 0)
		model::bindMaterial(parent->materials[materialIndex]);

	vao.bind();
	glDrawElements(GL_TRIANGLES, indiceSize, GL_UNSIGNED_INT, 0);
	VAO::unbind();

	glActiveTexture(GL_TEXTURE0);
}

void AssetModel::draw(ShaderProgram& shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i]->draw(shader);

	// all texture unbind
	for (unsigned int i = 0; i < MESH_TEXTURE_TYPE_SIZE; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		Texture::unbind();
	}

	glActiveTexture(GL_TEXTURE0);
}

void AssetModel::loadModel(string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "Error: Can't load model: " << importer.GetErrorString() << endl;
		return;
	}

	// resource path
	directory = path.substr(0, path.find_last_of('/'));

	// load all materials
	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* material = scene->mMaterials[i];
		struct Material* uMat = new Material();

		// load textures
		for (unsigned int i = 0; i < MESH_TEXTURE_TYPE_SIZE; i++)
		{
			if (material->GetTextureCount(textureTypes[i]) == 0)
			{
				uMat->texIndex[i] = -1;
				continue;
			}

			// get texture location
			aiString str;
			material->GetTexture(textureTypes[i], 0, &str);
			std::string str_s(str.C_Str());

			// load texture
			ModelTexture* tex = new ModelTexture();
			tex->texture = Texture::cacheImage((directory + '/' + str_s).c_str());
			tex->type = (TextureType)i;

			// set textureIndex
			uMat->texIndex[i] = textures.size();
			textures.push_back(tex);
		}

		// uniform block material setup
		fillMaterialColor(uMat->ambient, MaterialColorType::AMBIENT, material, 0.2f, 0.2f, 0.2f, 1.0f);
		fillMaterialColor(uMat->diffuse, MaterialColorType::DIFFUSE, material, 0.3f, 0.3f, 0.3f, 1.0f);
		fillMaterialColor(uMat->specular, MaterialColorType::SPECULAR, material, 0.0f, 0.0f, 0.0f, 1.0f);
		fillMaterialColor(uMat->emissive, MaterialColorType::EMISSIVE, material, 0.0f, 0.0f, 0.0f, 1.0f);

		// get shininess
		float shininess = 0.0;
		aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess);
		uMat->shininess = shininess;

		// add to material dictionary
		materials.push_back(uMat);
	}

	// load all meshes
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];
		meshes.push_back(new Mesh(this, mesh, scene));
	}
}

AssetModel::~AssetModel()
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		delete meshes[i];
	for (unsigned int i = 0; i < materials.size(); i++)
		delete materials[i];
	for (unsigned int i = 0; i < textures.size(); i++)
		delete textures[i];
}