#include <vector>
#include "balltracer.h"
#include "util/util.h"

#define TRACER_Y 0.16f
#define LINE_POINTS 4
#define CIRCLE_POINTS 30

namespace balltracer
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec4 color;
	};

	static Vertex makeVertex(float x, float y, float z, int color)
	{
		float a = ((color >> 24) & 0xff) / 255.0f;
		float r = ((color >> 16) & 0xff) / 255.0f;
		float g = ((color >> 8) & 0xff) / 255.0f;
		float b = (color & 0xff) / 255.0f;
		Vertex v = { {x, y, z}, {r, g, b, a} };
		return v;
	}
}

using namespace commoncg;
using namespace balltracer;


void BallTracer::init()
{
	// tracing line
	beamVao.create();
	beamVbo.create();
	beamVao.use();
	beamVbo.buffer(SIZEOF(Vertex, LINE_POINTS), nullptr, GL_DYNAMIC_DRAW);
	beamVao.attr(0, 3, GL_FLOAT, SIZEOF(float, 7), 0);
	beamVao.attr(1, 4, GL_FLOAT, SIZEOF(float, 7), SIZEOF(float, 3));

	// ball circle
	circleVao.create();
	circleVbo.create();
	circleVao.use();

	std::vector<Vertex> circleVerts;
	float dt = 2 * M_PI / CIRCLE_POINTS;
	for (int i = 0; i < CIRCLE_POINTS; i++)
	{
		float x = BALL_RADIUS * cos(i * dt);
		float y = BALL_RADIUS * sin(i * dt);
		circleVerts.push_back(makeVertex(x, 0, y, 0xaaffffff));
	}

	circleVbo.buffer(sizeof(Vertex) * circleVerts.size(), &circleVerts[0], GL_STATIC_DRAW);
	circleVao.attr(0, 3, GL_FLOAT, SIZEOF(float, 7), 0);
	circleVao.attr(1, 4, GL_FLOAT, SIZEOF(float, 7), SIZEOF(float, 3));
	VAO::unbind();

	// prepare shader
	ShaderProgram::push();
	beamShader.addShader("res/shader/ray.vert", GL_VERTEX_SHADER);
	beamShader.addShader("res/shader/ray.frag", GL_FRAGMENT_SHADER);
	beamShader.load();
	beamShader.use();
	beamShader.setUniform("model", glm::mat4(1.0f));
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

	glm::vec2 ballPos;
	glm::vec2 collisionVecEnd;

	if (traceRes.hitTargetBall == nullptr)
	{
		ballPos = traceRes.hitTimeBallPos;
		collisionVecEnd = ballPos + 0.5f * glm::normalize(glm::reflect(ballPos - position, traceRes.normal));
	} 
	else
	{
		ballPos = traceRes.hitTargetBall->position;
		collisionVecEnd = ballPos + 0.5f * glm::normalize(ballPos - traceRes.hitTimeBallPos);
	}

	hitBallPos = glm::vec3(traceRes.hitTimeBallPos.x, TRACER_Y, traceRes.hitTimeBallPos.y);
	traceRes.hitTimeBallPos -= BALL_RADIUS * glm::normalize(traceRes.hitTimeBallPos - position);

	balltracer::Vertex data[] =
	{
		makeVertex(position.x, TRACER_Y, position.y, 0x77ffffff),
		makeVertex(traceRes.hitTimeBallPos.x, TRACER_Y, traceRes.hitTimeBallPos.y, 0x77ffffff),
		makeVertex(ballPos.x, TRACER_Y, ballPos.y, 0xaaffffff),
		makeVertex(collisionVecEnd.x, TRACER_Y, collisionVecEnd.y, 0x00ffffff)
	};

	beamVao.use();
	beamVbo.buffer(SIZEOF(Vertex, LINE_POINTS), data, GL_DYNAMIC_DRAW);
	beamVbo.unbind();
	VAO::unbind();
	visible = true;
}

void BallTracer::draw()
{
	if (!visible)
		return;

	glDisable(GL_DEPTH_TEST);
	ShaderProgram::push();
	beamShader.use();

	// beams
	glLineWidth(4.0f);
	beamShader.setUniform("model", glm::mat4(1.0f));
	beamVao.use();
	glDrawArrays(GL_LINES, 0, LINE_POINTS);

	// ball ghost
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(2, 0xAAAA);
	glLineWidth(1.5f);
	beamShader.setUniform("model", glm::translate(glm::mat4(1.0f), hitBallPos));
	circleVao.use();
	glDrawArrays(GL_LINE_LOOP, 0, CIRCLE_POINTS);
	VAO::unbind();
	glDisable(GL_LINE_STIPPLE);

	ShaderProgram::pop();
	glEnable(GL_DEPTH_TEST);
	glLineWidth(DEFAULT_LINE_WIDTH);
}