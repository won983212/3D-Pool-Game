#include <iostream>
#include <vector>
#include "model.h"
#include "ball.h"
#include "../util/util.h"

using namespace std;
using namespace model;
using namespace glm;
using namespace commoncg;

const struct Material ballMaterial = 
{
	{ 0.8f, 0.8f, 0.8f, 1.0f },
	{ 1.0f, 1.0f, 1.0f, 1.0f },
	{ 0.6f, 0.6f, 0.6f, 1.0f },
	{ 0.0f, 0.0f, 0.0f, 0.0f },
	{ 0, -1, -1, -1 },
	600.0f
};

static void addSphereVertex(vector<Vertex>& vertices, int iPitch, int iYaw)
{
	const float pitchStep = M_PI / BALL_RENDER_STEP;
	const float yawStep = 2 * M_PI / BALL_RENDER_STEP;
	const float texStep = 1.0f / BALL_RENDER_STEP;

	vec3 normal;
	float pitch = pitchStep * (iPitch - BALL_RENDER_STEP / 2);
	float yaw = yawStep * iYaw;

	normal.x = cos(pitch) * cos(yaw);
	normal.y = sin(pitch);
	normal.z = cos(pitch) * sin(yaw);

	Vertex vert;
	vert.position = BALL_RADIUS * normal;
	vert.normal = normal;
	vert.texCoord = vec2(1 - texStep * iYaw, 1 - texStep * iPitch);
	vertices.push_back(vert);
}

void Ball::init()
{
	if (BALL_RENDER_STEP < 2)
	{
		std::cout << "Error: Ball::BALL_RENDER_STEP < 2" << std::endl;
		return;
	}

	vector<Vertex> vertices;

	vao.create();
	vbo.create();
	vao.bind();

	// prepare vertices
	for (int i = 0; i < BALL_RENDER_STEP; i++) // pitch
	{
		for (int j = 0; j < BALL_RENDER_STEP; j++) // yaw
		{
			// top triangle
			addSphereVertex(vertices, i, j);
			addSphereVertex(vertices, i + 1, j);
			addSphereVertex(vertices, i, j + 1);

			// bottom triangle
			addSphereVertex(vertices, i + 1, j);
			addSphereVertex(vertices, i + 1, j + 1);
			addSphereVertex(vertices, i, j + 1);
		}
	}

	verticesSize = vertices.size();
	vbo.buffer(verticesSize * sizeof(Vertex), &vertices[0]);

	vao.attr(0, 3, GL_FLOAT, sizeof(Vertex), 0);
	vao.attr(1, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, normal));
	vao.attr(2, 2, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, texCoord));

	vbo.unbind();
	VAO::unbind();
}

void Ball::draw(ShaderProgram& shader)
{
	// use GL_TEXTURE0 as texture_diffuse
	shader.setUniform(textureUniformNames[(int)TextureType::DIFFUSE].c_str(), 0);

	// bind material
	model::bindMaterial(&ballMaterial);

	vao.bind();
	glDrawArrays(GL_TRIANGLES, 0, verticesSize);
	VAO::unbind();
}