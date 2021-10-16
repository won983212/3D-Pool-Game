#include "balltracer.h"
#include "util/util.h"

using namespace commoncg;

void BallTracer::init()
{
	glm::vec3 initialVectors[] = {{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }};

	beamVao.create();
	beamVbo.create();
	beamVao.use();
	beamVbo.buffer(SIZEOF(glm::vec3, 4), initialVectors, GL_DYNAMIC_DRAW);
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

void BallTracer::update()
{
	if (direction.x == 0 && direction.y == 0)
	{
		visible = false;
		return;
	}

	RaytraceResult traceRes = table.getRaytracedBall(position, direction);
	if (!traceRes.hit)
	{
		visible = false;
		return;
	}

	const float y = 0.01f;
	glm::vec2 ballPos = traceRes.hitTargetBall->position;
	glm::vec2 collisionVec = ballPos + glm::normalize(ballPos - traceRes.hitTimeBallPos);
	glm::vec3 data[] = 
	{
		{ position.x, y, position.y },
		{ traceRes.hitTimeBallPos.x, y, traceRes.hitTimeBallPos.y },
		{ ballPos.x, y, ballPos.y },
		{ collisionVec.x, y, collisionVec.y }
	};

	beamVao.use();
	beamVbo.buffer(SIZEOF(glm::vec3, 4), data, GL_DYNAMIC_DRAW);
	beamVbo.unbind();
	VAO::unbind();
	visible = true;
}

// TODO Draw tracing ball too.
void BallTracer::draw()
{
	if (!visible)
		return;

	ShaderProgram::push();
	beamShader.use();
	beamVao.use();
	glDrawArrays(GL_LINES, 0, 4);
	VAO::unbind();
	ShaderProgram::pop();
}