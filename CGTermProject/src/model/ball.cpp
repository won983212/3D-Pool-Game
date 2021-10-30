#include <iostream>
#include <vector>
#include "model.h"
#include "ball.h"
#include "../util/util.h"

using namespace std;
using namespace model;
using namespace glm;
using namespace commoncg;

static void AddSphereVertex(vector<Vertex>& vertices, int i_pitch, int i_yaw, int step, float radius)
{
	const float pitch_step = M_PI / step;
	const float yaw_step = 2 * M_PI / step;
	const float tex_step = 1.0f / step;

	vec3 normal;
	const float pitch = pitch_step * (i_pitch - step / 2);
	const float yaw = yaw_step * i_yaw;

	normal.x = cos(pitch) * cos(yaw);
	normal.y = sin(pitch);
	normal.z = cos(pitch) * sin(yaw);

	Vertex vert;
	vert.position = radius * normal;
	vert.normal = normal;
	vert.tex_coord = vec2(1 - tex_step * i_yaw, 1 - tex_step * i_pitch);
	vertices.push_back(vert);
}

void Ball::Init(float radius, int step)
{
	if (step < 2)
	{
		cout << "Error: Ball::BALL_RENDER_STEP < 2" << endl;
		return;
	}

	vector<Vertex> vertices;

	vao_.Create();
	vbo_.Create();
	vao_.Use();

	// prepare vertices
	for (int i = 0; i < step; i++) // pitch
	{
		for (int j = 0; j < step; j++) // yaw
		{
			// top triangle
			AddSphereVertex(vertices, i, j, step, radius);
			AddSphereVertex(vertices, i + 1, j, step, radius);
			AddSphereVertex(vertices, i, j + 1, step, radius);

			// bottom triangle
			AddSphereVertex(vertices, i + 1, j, step, radius);
			AddSphereVertex(vertices, i + 1, j + 1, step, radius);
			AddSphereVertex(vertices, i, j + 1, step, radius);
		}
	}

	vertices_size_ = vertices.size();
	vbo_.Buffer(vertices_size_ * sizeof(Vertex), &vertices[0]);

	vao_.Attrib(0, 3, GL_FLOAT, sizeof(Vertex), 0);
	vao_.Attrib(1, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, normal));
	vao_.Attrib(2, 2, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, tex_coord));

	vbo_.Unbind();
	VAO::Unbind();
}

void Ball::Draw() const
{
	vao_.Use();
	glDrawArrays(GL_TRIANGLES, 0, vertices_size_);
	VAO::Unbind();
}
