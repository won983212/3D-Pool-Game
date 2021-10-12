#include <iostream>
#include <vector>
#include "model.h"
#include "quad.h"
#include "../util/util.h"

using namespace std;
using namespace model;
using namespace glm;
using namespace commoncg;

void Quad::init(float width, float height, vec3 normal)
{
	float hW = width / 2;
	float hH = height / 2;

	vec3 right;
	if (normal == vec3(0, 1, 0))
		right = vec3(1, 0, 0);
	else
		right = normalize(cross(vec3(0, 1, 0), normal));

	vec3 up = normalize(cross(normal, right));

	vector<Vertex> vertices;

	vao.create();
	vbo.create();
	vao.bind();

	Vertex v;
	v.normal = normalize(normal);

	// top triangle
	v.position = -hW * right - hH * up;
	v.texCoord = vec2(0, 0);
	vertices.push_back(v);

	v.position = hW * right - hH * up;
	v.texCoord = vec2(1, 0);
	vertices.push_back(v);

	v.position = -hW * right + hH * up;
	v.texCoord = vec2(0, 1);
	vertices.push_back(v);

	// bottom triangle
	v.position = hW * right - hH * up;
	v.texCoord = vec2(1, 0);
	vertices.push_back(v);

	v.position = hW * right + hH * up;
	v.texCoord = vec2(1, 1);
	vertices.push_back(v);

	v.position = -hW * right + hH * up;
	v.texCoord = vec2(0, 1);
	vertices.push_back(v);

	verticesSize = vertices.size();
	vbo.buffer(verticesSize * sizeof(Vertex), &vertices[0]);

	vao.attr(0, 3, GL_FLOAT, sizeof(Vertex), 0);
	vao.attr(1, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, Vertex::normal));
	vao.attr(2, 2, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, Vertex::texCoord));

	vbo.unbind();
	VAO::unbind();
}

void Quad::draw()
{
	vao.bind();
	glDrawArrays(GL_TRIANGLES, 0, verticesSize);
	VAO::unbind();
}