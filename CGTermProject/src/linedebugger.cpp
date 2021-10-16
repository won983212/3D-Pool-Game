#include <iostream>
#include "linedebugger.h"
#include "util/util.h"

using namespace commoncg;

void LineDebugger::init(int lines)
{
    points = new linedebugger::Vertex[lines * 2];
    pointSize = lines * 2;
    memset(points, 0, sizeof(linedebugger::Vertex) * pointSize);

    beamVao.create();
    beamVbo.create();
    beamVao.use();
    beamVbo.buffer(SIZEOF(linedebugger::Vertex, pointSize), points, GL_DYNAMIC_DRAW);
    beamVao.attr(0, 3, GL_FLOAT, SIZEOF(float, 7), 0);
    beamVao.attr(1, 4, GL_FLOAT, SIZEOF(float, 7), SIZEOF(float, 3));
    VAO::unbind();

    ShaderProgram::push();
    beamShader.addShader("res/shader/ray.vert", GL_VERTEX_SHADER);
    beamShader.addShader("res/shader/ray.frag", GL_FRAGMENT_SHADER);
    beamShader.load();
    beamShader.use();
    beamShader.setUniform("model", glm::mat4(1.0f));
    ShaderProgram::pop();
}

void LineDebugger::clear()
{
    len = 0;
}

void LineDebugger::add(float x, float y, float z)
{
    add(x, y, z, color);
}

void LineDebugger::add(float x, float y, float z, int color)
{
    if (len >= pointSize)
    {
        std::cout << "Error: Full size points" << std::endl;
        return;
    }

    float a = ((color >> 24) & 0xff) / 255.0f;
    float r = ((color >> 16) & 0xff) / 255.0f;
    float g = ((color >> 8) & 0xff) / 255.0f;
    float b = (color & 0xff) / 255.0f;

    linedebugger::Vertex v = { {x, y, z}, {r, g, b, a} };
    points[len++] = v;
}

void LineDebugger::update()
{
    beamVao.use();
    beamVbo.buffer(SIZEOF(linedebugger::Vertex, pointSize), points, GL_DYNAMIC_DRAW);
    beamVbo.unbind();
    VAO::unbind();
}

void LineDebugger::draw()
{
    ShaderProgram::push();
    beamShader.use();
    beamVao.use();
    glDrawArrays(GL_LINES, 0, pointSize);
    VAO::unbind();
    ShaderProgram::pop();
}