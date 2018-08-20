#pragma once

#include "GLObject.h"

class Unit : public GLObject
{
public:
	Unit(const char*);
	Unit(glm::vec3&);
	~Unit();

	void render(GLuint&) override;
	void drawSelectedMarker(GLuint&);

	bool hasDestination();
	void addDestination(glm::vec3&);
	void addToDestinationQueue(glm::vec3&);
	void setDestination(glm::vec3);
	void popDestination();
	glm::vec3 getDestination();
	bool takeDamage(float);
	void targetNearestEntity(std::vector<Unit*>);

	GLObject* target;
	bool friendly;
	bool isDead;
	float speed;
private:
	void drawHealthBar(GLuint&);

	std::vector<glm::vec3> destinations;
	float health;
};