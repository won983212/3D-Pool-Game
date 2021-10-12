#include <iostream>
#include "../gfx/texture.h"
#include "assetmodel.h"
#include "assimp/pbrmaterial.h"


using namespace model;
using namespace commoncg;
using namespace std;


static string getTexPathAssimp(aiMaterial* material, aiTextureType type, unsigned int index)
{
	if (material->GetTextureCount(type) > 0)
	{
		aiString ai_filename;
		if (material->GetTexture(type, index, &ai_filename) == AI_SUCCESS)
			return ai_filename.C_Str();
	}
	return "";
}

static glm::vec4 getVecAssimp(aiMaterial* material, glm::vec4 defaultValue, const char* key, unsigned int type, unsigned int index)
{
	aiColor4D ai_color;
	if (material->Get(key, type, index, ai_color) == AI_SUCCESS)
		return glm::vec4(ai_color.r, ai_color.g, ai_color.b, ai_color.a);
	return defaultValue;
}

static float getFloatAssimp(aiMaterial* material, float defaultValue, const char* key, unsigned int type, unsigned int index)
{
	float data;
	if (material->Get(key, type, index, data) == AI_SUCCESS)
		return data;
	return defaultValue;
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
		shader.setUniform(name.c_str(), texturePBRIndexes[i]);
		glActiveTexture(GL_TEXTURE0 + texturePBRIndexes[i]);
		texture->texture->bind();
	}

	// bind material to shader
	if (materialIndex >= 0)
		model::bindMaterial(parent->materials[materialIndex]);

	vao.bind();
	glDrawElements(GL_TRIANGLES, indiceSize, GL_UNSIGNED_INT, 0);
	VAO::unbind();
}

void AssetModel::draw(ShaderProgram& shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i]->draw(shader);

	// all texture unbind
	for (unsigned int i = 0; i < MESH_TEXTURE_TYPE_SIZE; i++)
	{
		glActiveTexture(GL_TEXTURE0 + texturePBRIndexes[i]);
		Texture::unbind();
	}

	glActiveTexture(GL_TEXTURE0);
}

void AssetModel::loadTexture(const aiScene* scene, string texPath, Material* uMat, int id)
{
	// load texture
	ModelTexture* tex = new ModelTexture();
	tex->texture = Texture::cacheImage((directory + '/' + texPath).c_str(), GL_REPEAT);
	tex->type = (TextureType)id;

	// set textureIndex
	uMat->texIndex[id] = textures.size();
	textures.push_back(tex);
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
		Material* uMat = new Material();

		for (int j = 0; j < MESH_TEXTURE_TYPE_SIZE; j++)
			uMat->texIndex[j] = -1;

		// load textures
		string albedoTexPath = getTexPathAssimp(material, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE);
		if (!albedoTexPath.empty())
			loadTexture(scene, albedoTexPath, uMat, 0);

		string mrTexPath = getTexPathAssimp(material, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE);
		if (!mrTexPath.empty())
		{
			int dashIdx = mrTexPath.find("-");
			if (dashIdx == string::npos)
			{
				loadTexture(scene, mrTexPath, uMat, 1);
			}
			else
			{
				loadTexture(scene, mrTexPath.substr(0, dashIdx), uMat, 1);
				if(mrTexPath.size() > dashIdx + 1)
					loadTexture(scene, mrTexPath.substr(dashIdx + 1), uMat, 2);
			}
		}

		string normalTexPath = getTexPathAssimp(material, aiTextureType_NORMALS, 0);
		if (!normalTexPath.empty())
			loadTexture(scene, normalTexPath, uMat, 3);

		// uniform block material setup
		uMat->albedo = getVecAssimp(material, glm::vec4(1.0f), AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_FACTOR);
		uMat->metallic = getFloatAssimp(material, 1.0f, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR);
		uMat->roughness = getFloatAssimp(material, 1.0f, AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR);
		uMat->ao = DEFAULT_AO;

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