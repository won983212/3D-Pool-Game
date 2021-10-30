#include "model.h"
#include "../gfx/vbo.h"
#include "../util/util.h"

using namespace commoncg;

// material ubo
VBO u_material(GL_UNIFORM_BUFFER);

void model::BindMaterial(const struct Material* material)
{
	if (!u_material.IsCreated())
		u_material.Create();
	u_material.BindBufferRange(UNIFORM_BINDING_MATERIAL, 0, sizeof(struct Material));
	u_material.Buffer(sizeof(struct Material), material, GL_DYNAMIC_DRAW);
	u_material.Unbind();
}
