#include <iostream>
#include <vector>
#include "model.h"
#include "ball.h"
#include "../util/util.h"

using namespace std;
using namespace model;
using namespace glm;
using namespace commoncg;

static void addSphereVertex(vector<Vertex>& vertices, int iPitch, int iYaw, int step, float radius)
{
	const float pitchStep = M_PI / step;
	const float yawStep = 2 * M_PI / step;
	const float texStep = 1.0f / step;

	vec3 normal;
	float pitch = pitchStep * (iPitch - step / 2);
	float yaw = yawStep * iYaw;

	normal.x = cos(pitch) * cos(yaw);
	normal.y = sin(pitch);
	normal.z = cos(pitch) * sin(yaw);

	Vertex vert;
	vert.position = radius * normal;
	vert.normal = normal;
	vert.texCoord = vec2(1 - texStep * iYaw, 1 - texStep * iPitch);
	vertices.push_back(vert);
}

void Ball::init(float radius, int step)
{
	if (step < 2)
	{
		cout << "Error: Ball::BALL_RENDER_STEP < 2" << endl;
		return;
	}

	vector<Vertex> vertices;

	vao.create();
	vbo.create();
	vao.bind();

	// prepare vertices
	for (int i = 0; i < step; i++) // pitch
	{
		for (int j = 0; j < step; j++) // yaw
		{
			// top triangle
			addSphereVertex(vertices, i, j, step, radius);
			addSphereVertex(vertices, i + 1, j, step, radius);
			addSphereVertex(vertices, i, j + 1, step, radius);

			// bottom triangle
			addSphereVertex(vertices, i + 1, j, step, radius);
			addSphereVertex(vertices, i + 1, j + 1, step, radius);
			addSphereVertex(vertices, i, j + 1, step, radius);
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

void Ball::draw()
{
	vao.bind();
	glDrawArrays(GL_TRIANGLES, 0, verticesSize);
	VAO::unbind();
}