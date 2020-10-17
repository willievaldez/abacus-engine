#pragma once

#include <glm/vec3.hpp> // vec3
#include <time.h> // clock_t

class Unit;
class PointLight;

class Player
{
public:
	Player(const glm::vec3& pos);

	void Update(const clock_t& tick);

	Unit* GetUnit() { return m_unit; };
	PointLight* GetLight() { return m_light; };
private:
	Unit* m_unit = nullptr;
	PointLight* m_light = nullptr;
};