#pragma once

#include <glm/vec3.hpp>

#include "Structure.h"
#include "Unit.h"

class Spawner : public Structure
{
public:
	Spawner(glm::vec3&, bool);
	~Spawner();

	Unit* spawn(clock_t);

private:
	GLint spawnedTexture;
};