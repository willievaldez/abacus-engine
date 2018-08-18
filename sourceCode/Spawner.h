#pragma once

#include <glm/vec3.hpp>
#include <chrono>

#include "Enemy.h"

class Spawner
{
public:
	Spawner(glm::vec3&);
	~Spawner();

	Enemy* spawn(clock_t);

private:
	glm::vec3 position;
	std::chrono::milliseconds period;
	clock_t lastUpdateTime;
};