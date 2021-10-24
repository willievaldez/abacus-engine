#include <GLWrapper/PointLight.h>

// TODO: GLWrapper includes Game
#include <Game/Level.h>

PointLight* PointLight::Create(const glm::vec3& p, float i, float r, float ar)
{
	PointLight* light = new PointLight(p, i, r, ar);
	Level::Get()->AddLight(light);
	return light;
}

PointLight::~PointLight()
{
	Level::Get()->RemoveLight(this);
}
