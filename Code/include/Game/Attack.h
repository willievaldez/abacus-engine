#pragma once

#include <GLWrapper/GLObject.h>

class Attack : public GLObject
{
public:
	Attack(const char* assetName) : GLObject(assetName) {};
	virtual bool Update() = 0;
	float GetCost() const { return m_castCost; };
protected:
	float m_dmg = 5.0f;
	float m_castCost = 1.0f;
};

class RangedAttack : public Attack
{
public:
	RangedAttack(const char* assetName) : Attack(assetName) {};
	bool Update() override;
private:
	float m_speed = 0.3f;
	float m_radius = 0.5f;
};