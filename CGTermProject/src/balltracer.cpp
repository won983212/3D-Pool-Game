#include "balltracer.h"
#include "util/util.h"

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
	beamVao.create();
	beamVbo.create();
	beamVao.use();
	beamVbo.buffer(SIZEOF(Vertex, 4), nullptr, GL_DYNAMIC_DRAW);
	beamVao.attr(0, 3, GL_FLOAT, SIZEOF(float, 7), 0);
	beamVao.attr(1, 4, GL_FLOAT, SIZEOF(float, 7), SIZEOF(float, 3));
	VAO::unbind();

	ShaderProgram::push();
	beamShader.addShader("res/shader/ray.vert", GL_VERTEX_SHADER);
	beamShader.addShader("res/shader/ray.frag", GL_FRAGMENT_SHADER);
	beamShader.load();
	beamShader.use();
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
	glm::vec2 collisionVec = ballPos + 0.5f * glm::normalize(ballPos - traceRes.hitTimeBallPos);
	balltracer::Vertex data[] =
	{
		makeVertex(position.x, y, position.y, 0xaaffffff),
		makeVertex(traceRes.hitTimeBallPos.x, y, traceRes.hitTimeBallPos.y, 0xaaffffff),
		makeVertex(ballPos.x, y, ballPos.y, 0xaaffffff),
		makeVertex(collisionVec.x, y, collisionVec.y, 0x00ffffff)
	};

	beamVao.use();
	beamVbo.buffer(SIZEOF(Vertex, 4), data, GL_DYNAMIC_DRAW);
	beamVbo.unbind();
	VAO::unbind();
	visible = true;
}

// TODO Draw tracing ball too.
void BallTracer::draw()
{
	if (!visible)
		return;

	glLineWidth(3.0f);
	glDisable(GL_DEPTH_TEST);
	ShaderProgram::push();

	beamShader.use();
	beamVao.use();
	glDrawArrays(GL_LINES, 0, 4);
	VAO::unbind();

	ShaderProgram::pop();
	glEnable(GL_DEPTH_TEST);
	glLineWidth(DEFAULT_LINE_WIDTH);
}