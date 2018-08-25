#pragma once

#include "GLObject.h"
#include <chrono>

enum StructureType
{
	SPAWNER,
	TURRET,
	GENERIC_TYPE
};

class Structure : public GLObject
{
public:
	Structure(glm::vec3&, GLint);
	~Structure();

	void update(clock_t);
	void render() override;
	void build(float);

	bool built;
	StructureType STRUCTURE_TYPE;

protected:
	bool friendly;
	float health;
	float period;
	clock_t lastUpdateTime;
	void drawHealthBar();
};