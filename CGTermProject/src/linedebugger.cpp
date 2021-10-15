#include "linedebugger.h"
#include "util/util.h"

using namespace commoncg;

void LineDebugger::init(int lines)
{
    points = new glm::vec3[lines * 2];
    pointSize = lines * 2;
    memset(points, 0, sizeof(glm::vec3) * pointSize);

    beamVao.create();
    beamVbo.create();
    beamVao.use();
    beamVbo.buffer(SIZEOF(glm::vec3, pointSize), points, GL_DYNAMIC_DRAW);
    beamVao.attr(0, 3, GL_FLOAT, SIZEOF(float, 3), 0);
    VAO::unbind();

    ShaderProgram::push();
    beamShader.addShader("res/shader/ray.vert", GL_VERTEX_SHADER);
    beamShader.addShader("res/shader/ray.frag", GL_FRAGMENT_SHADER);
    beamShader.load();
    beamShader.use();
    beamShader.setUniform("color", glm::vec3(1.0f, 0.0f, 0.0f));
    ShaderProgram::pop();
}

void LineDebugger::setColor(int color)
{
    float r = ((color >> 16) & 0xff) / 255.0f;
    float g = ((color >> 8) & 0xff) / 255.0f;
    float b = (color & 0xff) / 255.0f;

    ShaderProgram::push();
    beamShader.use();
    beamShader.setUniform("color", glm::vec3(r, g, b));
    ShaderProgram::pop();
}

void LineDebugger::update()
{
    beamVao.use();
    beamVbo.buffer(SIZEOF(glm::vec3, pointSize), points, GL_DYNAMIC_DRAW);
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