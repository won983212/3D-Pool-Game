#include "dirlight.h"
#include "util/util.h"

DirectionalLight::DirectionalLight() 
    :uboLight(GL_UNIFORM_BUFFER), data()
{ };

void DirectionalLight::create()
{
    uboLight.create();
}

void DirectionalLight::update() const
{
    uboLight.bindBufferRange(UNIFORM_BINDING_LIGHT, 0, sizeof(data));
    uboLight.buffer(sizeof(data), &data);
    uboLight.unbind();
}