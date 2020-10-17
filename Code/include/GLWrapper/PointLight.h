#pragma once

#include <glm/vec3.hpp> // vec3

class PointLight
{
public:
	static PointLight* Create(const glm::vec3& p, float i = 1.0f, float r = 100.0f);
	~PointLight();

	glm::vec3 pos;
	float intensity;
	float radius;
private:
	PointLight(const glm::vec3& p, float i = 1.0f, float r = 100.0f)
		: pos(p)
		, intensity(i)
		, radius(r) {};
};
