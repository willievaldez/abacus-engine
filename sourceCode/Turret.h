#pragma once

#include <glm/vec3.hpp>

#include "Structure.h"
#include "Unit.h"

class Turret : public Structure
{
public:
	Turret(glm::vec3&);
	~Turret();

	Unit* spawn(clock_t);
	void damageEnemyWithinRange(std::vector<Unit*>);

private:
	float power;
	float range;

};