#pragma once

#include "GLObject.h"

class Unit : public GLObject
{
public:
	Unit(const char*);
	~Unit();

	void render(GLuint&) override;

	bool hasDestination();
	void addDestination(glm::vec3&);
	void addToDestinationQueue(glm::vec3&);
	void setDestination(glm::vec3);
	void popDestination();
	glm::vec3 getDestination();
	bool takeDamage(float);
private:
	void drawHealthBar(GLuint&);

	std::vector<glm::vec3> destinations;
	float health;
};