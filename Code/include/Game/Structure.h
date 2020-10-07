#pragma once

#include <GLWrapper/GLObject.h>
#include <chrono>

enum StructureType
{
	SPAWNER,
	TURRET,
	GENERIC_TYPE
};

class Tile;

class Structure : public GLObject
{
public:
	Structure(glm::vec3&, Asset*);
	~Structure();

	virtual void Update(clock_t) {};
	//void build(float);

	//bool built;
	//StructureType STRUCTURE_TYPE;

protected:
	//bool friendly;
	//float health;
	//float period;
};