#pragma once

#include "GLObject.h"
#include "Action.h"

#include <chrono>

class Action;

class Unit : public GLObject
{
public:
	Unit(GLint, bool);
	Unit(const char*, bool);
	Unit(glm::vec3&, bool);
	~Unit();

	void render() override;
	void renderGravestone();
	void update(clock_t);

	bool takeDamage(float);
	void addAction(Action*, bool);
	void drawActions();

	bool friendly;
	bool isDead;
	float speed;
private:
	void drawHealthBar();
	Action* idleAction;
	Action* currentAction;
	float health;
	//std::vector<Action*> actions;
};