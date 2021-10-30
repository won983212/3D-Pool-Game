#include <iostream>
#include <vector>
#include "model.h"
#include "quad.h"

using namespace std;
using namespace model;
using namespace glm;
using namespace commoncg;

void Quad::Init(float width, float height, vec3 normal)
{
	float half_w = width / 2;
	float half_h = height / 2;

	vec3 right;
	if (normal == vec3(0, 1, 0))
		right = vec3(1, 0, 0);
	else
		right = normalize(cross(vec3(0, 1, 0), normal));

	vec3 up = normalize(cross(normal, right));

	vector<Vertex> vertices;

	vao_.Create();
	vbo_.Create();
	vao_.Use();

	Vertex v;
	v.normal = normalize(normal);

	// top triangle
	v.position = -half_w * right - half_h * up;
	v.tex_coord = vec2(0, 0);
	vertices.push_back(v);

	v.position = half_w * right - half_h * up;
	v.tex_coord = vec2(1, 0);
	vertices.push_back(v);

	v.position = -half_w * right + half_h * up;
	v.tex_coord = vec2(0, 1);
	vertices.push_back(v);

	// bottom triangle
	v.position = half_w * right - half_h * up;
	v.tex_coord = vec2(1, 0);
	vertices.push_back(v);

	v.position = half_w * right + half_h * up;
	v.tex_coord = vec2(1, 1);
	vertices.push_back(v);

	v.position = -half_w * right + half_h * up;
	v.tex_coord = vec2(0, 1);
	vertices.push_back(v);

	vertices_size_ = vertices.size();
	vbo_.Buffer(vertices_size_ * sizeof(Vertex), &vertices[0]);

	vao_.Attrib(0, 3, GL_FLOAT, sizeof(Vertex), 0);
	vao_.Attrib(1, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, Vertex::normal));
	vao_.Attrib(2, 2, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, Vertex::tex_coord));

	vbo_.Unbind();
	VAO::Unbind();
}

void Quad::Draw() const
{
	vao_.Use();
	glDrawArrays(GL_TRIANGLES, 0, vertices_size_);
	VAO::Unbind();
}
