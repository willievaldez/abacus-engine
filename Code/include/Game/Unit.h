#pragma once

#include <GLWrapper/GLObject.h>
#include <Game/Action.h>

#include <chrono>

class Action;

class Unit : public GLObject
{
public:
	static Unit* Create(const char*);
	static Unit* Create(Asset*);
	~Unit();

	void Render() override;
	//void renderGravestone();
	void Update(clock_t);

	void BasicAttack(const glm::vec3&, const glm::vec3&);
	bool TakeDamage(float);
	void addAction(Action*, bool);
	void drawActions();
	float GetHealth() { return m_currentHealth; };
	void GetMovePosition(const::glm::vec3& direction, glm::vec3& destination);

	//bool friendly;
	//bool isDead;
private:
	Unit(Asset*);

	Action* idleAction;
	Action* currentAction;
	float m_currentHealth;
	int m_maxHealth = 100;
	int m_unitId = -1;
	float m_speed = 4.0f;
	float m_manaDepletionPerSec = 0.25f;
	clock_t m_lastFrameTick;
	int m_animationFrame;
};