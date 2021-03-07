#pragma once

#include <GLWrapper/GLObject.h>
#include <chrono>

class Unit;

class Structure : public GLObject
{
public:
	Structure(const glm::vec3&, Asset*);
	~Structure();

	virtual void Update(clock_t) {};
	virtual void Interact(Unit* player) {};
};