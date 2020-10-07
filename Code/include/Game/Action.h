#pragma once

#include <Game/Level.h>

#include <chrono>

enum class ActionType
{
	Idle,
	Target,
	Move,
	BuildOrRepair
};

class Unit;
class Structure;
class Action // lawsuit
{
public:
	Action();
	~Action();

	virtual bool execute(clock_t&, Unit*) = 0;
	virtual void draw() = 0;

	Action* nextAction;
	ActionType type;

protected:
	clock_t lastUpdateInterval;
};

class TargetAction : public Action
{
public:
	TargetAction(Unit*);
	~TargetAction();

	bool execute(clock_t&, Unit*) override;
	void draw() override;

private:
	Unit * target;
};

class BuildOrRepairAction : public Action
{
public:
	BuildOrRepairAction(Structure*);
	~BuildOrRepairAction();

	bool execute(clock_t&, Unit*) override;
	void draw() override;

private:
	Structure* target;
};

class MoveAction : public Action
{
public:
	MoveAction(glm::vec3&);
	~MoveAction();

	bool execute(clock_t&, Unit*) override;
	void draw() override;

private:
	glm::vec3 destination;
};

class Unit;
class IdleDefendAction : public Action
{
public:
	IdleDefendAction(float);
	~IdleDefendAction();

	bool execute(clock_t&, Unit*) override;
	void draw() override;

private:
	float range;
};

class IdleAttackAction : public Action
{
public:
	IdleAttackAction();
	~IdleAttackAction();

	bool execute(clock_t&, Unit*) override;
	void draw() override;

};