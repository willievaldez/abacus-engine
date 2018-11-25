#pragma once

#include "GLObject.h"
#include "Action.h"
#include "Animation.h"

#include <chrono>

class Action;

class Unit : public GLObject
{
public:
	Unit(Asset*, bool);
	Unit(const char*, bool);
	Unit(glm::vec3&, bool);
	~Unit();

	void draw() override;
	//void renderGravestone();
	void update(clock_t);

	bool takeDamage(float);
	void addAction(Action*, bool);
	void drawActions();

	bool friendly;
	bool isDead;
	float speed;
private:
	//void drawHealthBar();
	Action* idleAction;
	Action* currentAction;
	float health;

	Animation* idleAnimation, *movingAnimation, *attackingAnimation;
};