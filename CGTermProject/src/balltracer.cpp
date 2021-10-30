#include <vector>
#include "balltracer.h"
#include "util/util.h"

constexpr float TracerY = 0.16f;
constexpr int LinePoints = 4;
constexpr int CirclePoints = 30;

namespace balltracer
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec4 color;
	};

	static Vertex MakeVertex(float x, float y, float z, unsigned int color)
	{
		float a = (color >> 24 & 0xff) / 255.0f;
		float r = (color >> 16 & 0xff) / 255.0f;
		float g = (color >> 8 & 0xff) / 255.0f;
		float b = (color & 0xff) / 255.0f;
		const Vertex v = {{x, y, z}, {r, g, b, a}};
		return v;
	}
}

using namespace commoncg;
using namespace balltracer;


void BallTracer::Init()
{
	// tracing line
	beam_vao_.Create();
	beam_vbo_.Create();
	beam_vao_.Use();
	beam_vbo_.Buffer(SIZEOF(Vertex, LinePoints), nullptr, GL_DYNAMIC_DRAW);
	beam_vao_.Attrib(0, 3, GL_FLOAT, SIZEOF(float, 7), 0);
	beam_vao_.Attrib(1, 4, GL_FLOAT, SIZEOF(float, 7), SIZEOF(float, 3));

	// ball circle
	circle_vao_.Create();
	circle_vbo_.Create();
	circle_vao_.Use();

	std::vector<Vertex> circle_verts;
	constexpr float dt = 2 * M_PI / CirclePoints;
	for (int i = 0; i < CirclePoints; i++)
	{
		const float x = BallRadius * cos(i * dt);
		const float y = BallRadius * sin(i * dt);
		circle_verts.push_back(MakeVertex(x, 0, y, 0xaaffffff));
	}

	circle_vbo_.Buffer(sizeof(Vertex) * circle_verts.size(), &circle_verts[0], GL_STATIC_DRAW);
	circle_vao_.Attrib(0, 3, GL_FLOAT, SIZEOF(float, 7), 0);
	circle_vao_.Attrib(1, 4, GL_FLOAT, SIZEOF(float, 7), SIZEOF(float, 3));
	VAO::Unbind();

	// prepare shader
	ShaderProgram::Push();
	beam_shader_.AddShader("res/shader/ray.vert", GL_VERTEX_SHADER);
	beam_shader_.AddShader("res/shader/ray.frag", GL_FRAGMENT_SHADER);
	beam_shader_.Load();
	beam_shader_.Use();
	beam_shader_.SetUniform("model", glm::mat4(1.0f));
	ShaderProgram::Pop();
}

void BallTracer::Update()
{
	if (direction_.x == 0.0f && direction_.y == 0.0f)
	{
		visible_ = false;
		return;
	}

	RaytraceResult trace_res = table_.GetRaytracedBall(position_, direction_);
	if (!trace_res.hit)
	{
		visible_ = false;
		return;
	}

	glm::vec2 target_ball_pos;
	glm::vec2 collision_vec_end;

	if (trace_res.hit_target_ball == nullptr)
	{
		target_ball_pos = trace_res.hit_ball_pos;
		collision_vec_end = target_ball_pos + 0.5f * normalize(reflect(target_ball_pos - position_, trace_res.normal));
	}
	else
	{
		target_ball_pos = trace_res.hit_target_ball->position_;
		collision_vec_end = target_ball_pos + 0.5f * normalize(target_ball_pos - trace_res.hit_ball_pos);
	}

	hit_ball_pos_ = glm::vec3(trace_res.hit_ball_pos.x, TracerY, trace_res.hit_ball_pos.y);
	trace_res.hit_ball_pos -= BallRadius * normalize(trace_res.hit_ball_pos - position_);

	const Vertex data[] =
	{
		MakeVertex(position_.x, TracerY, position_.y, 0x77ffffff),
		MakeVertex(trace_res.hit_ball_pos.x, TracerY, trace_res.hit_ball_pos.y, 0x77ffffff),
		MakeVertex(target_ball_pos.x, TracerY, target_ball_pos.y, 0xaaffffff),
		MakeVertex(collision_vec_end.x, TracerY, collision_vec_end.y, 0x00ffffff)
	};

	beam_vao_.Use();
	beam_vbo_.Buffer(SIZEOF(Vertex, LinePoints), data, GL_DYNAMIC_DRAW);
	beam_vbo_.Unbind();
	VAO::Unbind();
	visible_ = true;
}

void BallTracer::Draw() const
{
	if (!visible_)
		return;

	glDisable(GL_DEPTH_TEST);
	ShaderProgram::Push();
	beam_shader_.Use();

	// beams
	glLineWidth(4.0f);
	beam_shader_.SetUniform("model", glm::mat4(1.0f));
	beam_vao_.Use();
	glDrawArrays(GL_LINES, 0, LinePoints);

	// ball ghost
	glEnable(GL_LINE_STIPPLE);
	glLineStipple(2, 0xAAAA);
	glLineWidth(1.5f);
	beam_shader_.SetUniform("model", translate(glm::mat4(1.0f), hit_ball_pos_));
	circle_vao_.Use();
	glDrawArrays(GL_LINE_LOOP, 0, CirclePoints);
	VAO::Unbind();
	glDisable(GL_LINE_STIPPLE);

	ShaderProgram::Pop();
	glEnable(GL_DEPTH_TEST);
	glLineWidth(DefaultLineWidth);
}
