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

std::unordered_map<std::string, model::Texture> loaded_textures;


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

Mesh::Mesh(std::string directory, aiMesh* mesh, const aiScene* scene)
	: ebo(GL_ELEMENT_ARRAY_BUFFER), u_material(GL_UNIFORM_BUFFER), indiceSize(0)
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
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		// diffuse
		vector<model::Texture> diffuseMaps = loadMaterialTextures(directory, material, aiTextureType_DIFFUSE, TextureType::DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		// specular
		vector<model::Texture> specularMaps = loadMaterialTextures(directory, material, aiTextureType_SPECULAR, TextureType::SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		// normal
		vector<model::Texture> normalMaps = loadMaterialTextures(directory, material, aiTextureType_NORMALS, TextureType::NORMAL);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

		// height
		vector<model::Texture> heightMaps = loadMaterialTextures(directory, material, aiTextureType_HEIGHT, TextureType::HEIGHT);
		textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());


		// uniform block material setup
		fillMaterialColor(uMat.ambient, MaterialColorType::AMBIENT, material, 0.2f, 0.2f, 0.2f, 1.0f);
		fillMaterialColor(uMat.diffuse, MaterialColorType::DIFFUSE, material, 0.3f, 0.3f, 0.3f, 1.0f);
		fillMaterialColor(uMat.specular, MaterialColorType::SPECULAR, material, 0.0f, 0.0f, 0.0f, 1.0f);
		fillMaterialColor(uMat.emissive, MaterialColorType::EMISSIVE, material, 0.0f, 0.0f, 0.0f, 1.0f);

		// get shininess
		float shininess = 0.0;
		aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shininess);
		uMat.shininess = shininess;

		// send to buffer
		u_material.create();
		bindMaterial();
	}

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

vector<model::Texture> Mesh::loadMaterialTextures(std::string directory, aiMaterial* mat, aiTextureType type, TextureType textureType)
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
			textures.push_back(tex);
			loaded_textures.insert(std::make_pair(str.C_Str(), tex));
		}
	}

	uMat.texCount[(int)textureType] = textures.size();
	return textures;
}

void Mesh::draw(ShaderProgram& shader)
{
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		TextureType type = textures[i].type;
		string name = textureUniformNames[(int)type];
		
		shader.setUniform(name.c_str(), (int)i);
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	bindMaterial();

	vao.bind();
	glDrawElements(GL_TRIANGLES, indiceSize, GL_UNSIGNED_INT, 0);
	VAO::unbind();

	glActiveTexture(GL_TEXTURE0);
}

void Mesh::bindMaterial() const
{
	u_material.bindBufferRange(UNIFORM_BINDING_MATERIAL, 0, sizeof(struct Material));
	u_material.buffer(sizeof(uMat), &uMat, GL_DYNAMIC_DRAW);
	u_material.unbind();
}


void Model::draw(ShaderProgram& shader)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
		meshes[i].draw(shader);
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
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];
		meshes.push_back(Mesh(directory, mesh, scene));
	}
}