#pragma once

#include <glm/vec3.hpp>
#include <chrono>

#include "Unit.h"

class Spawner
{
public:
	Spawner(glm::vec3&);
	~Spawner();

	Unit* spawn(clock_t);

private:
	glm::vec3 position;
	float period;
	clock_t lastUpdateTime;
};