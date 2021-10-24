#pragma once

#include <glm/vec3.hpp> // vec3

class PointLight
{
public:
	static PointLight* Create(const glm::vec3& p, float i = 1.0f, float r = 100.0f, float ar = 0.0f);
	~PointLight();

	glm::vec3 pos;
	float intensity;
	float radius;
	float ambientRadius;
private:
	PointLight(const glm::vec3& p, float i, float r, float ar)
		: pos(p)
		, intensity(i)
		, radius(r)
		, ambientRadius(ar){};
};
