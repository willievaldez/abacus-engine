#pragma once

#include "GLObject.h"

class Unit : public GLObject
{
public:
	Unit(GLint);
	Unit(const char*);
	Unit(glm::vec3&);
	~Unit();

	void render() override;

	bool takeDamage(float);
	void targetNearestEntity(std::vector<GLObject*>, bool=false);

	GLObject* target;
	std::vector<glm::vec3> destinations;

	bool friendly;
	bool isDead;
	float speed;
private:
	void drawHealthBar();

	float health;
};