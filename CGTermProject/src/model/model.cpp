#include "model.h"
#include "../gfx/vbo.h"
#include "../util/util.h"

VBO uMaterial(GL_UNIFORM_BUFFER);

void model::bindMaterial(const struct Material* materials)
{
	if (!uMaterial.isCreated())
		uMaterial.create();
	uMaterial.bindBufferRange(UNIFORM_BINDING_MATERIAL, 0, sizeof(struct Material));
	uMaterial.buffer(sizeof(struct Material), materials, GL_DYNAMIC_DRAW);
	uMaterial.unbind();
}