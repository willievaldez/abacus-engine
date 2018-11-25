#pragma once

#include <glm/vec3.hpp>

#include "Structure.h"
#include "Unit.h"

class Unit;

class Spawner : public Structure
{
public:
	Spawner(glm::vec3&, Asset*);
	~Spawner();

	Unit* spawn(clock_t);

private:
	Asset* spawnAsset;
};