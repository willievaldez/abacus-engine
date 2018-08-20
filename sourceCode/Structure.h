#pragma once

#include "Unit.h" // Unit, GLObject

class Structure : public GLObject
{
public:
	Structure(glm::vec3&);
	~Structure();

	void render(GLuint&) override;
	void build(float);
	void damageEnemyWithinRange(std::vector<Unit*>);

	bool built;

private:
	float range;
	float health;
	float power;

	void drawHealthBar(GLuint&);
};