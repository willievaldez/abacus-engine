#pragma once

#include <glm/vec3.hpp>

#include <Game/Structure.h>
#include <Game/Unit.h>

class Unit;
class Turret : public Structure
{
public:
	Turret(glm::vec3&);
	~Turret();

	void damageEnemyWithinRange(std::vector<Unit*>);

private:
	float power;
	float range;

};