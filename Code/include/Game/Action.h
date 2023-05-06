#pragma once

#include <Game/Level.h>
#include <Utility/Utility.h>

#include <chrono>

class Action;

#define REGISTER_ACTION(TYPE_NAME, CLASS) REGISTER(Action, TYPE_NAME, CLASS)

template<typename T>
std::shared_ptr<Action> ActionFactory()
{
	return std::make_shared<T>();
}

class Unit;
class Structure;
class Action // lawsuit
{
public:
	Action();
	~Action();

	virtual bool Execute(clock_t&, Unit*) = 0;

	template<typename T>
	static size_t Register(const char* actionName)
	{
		auto foundType = AccessActions().find(actionName);
		assert(foundType == AccessActions().end());
		AccessActions()[actionName] = &ActionFactory<T>;
		return AccessActions().size();
	};

	std::shared_ptr<Action> nextAction;

	static std::shared_ptr<Action> CreateAction(const char* actionName);
	using ActionMap = std::unordered_map<std::string, std::shared_ptr<Action>(*)()>;

protected:
	clock_t lastUpdateInterval;

	static ActionMap& AccessActions() { static ActionMap actionMap; return actionMap; };

};
//
//class TargetAction : public Action
//{
//public:
//	TargetAction(Unit*);
//	~TargetAction();
//
//	bool Execute(clock_t&, Unit*) override;
//	void Render() override;
//
//private:
//	Unit* target;
//};
//
//class BuildOrRepairAction : public Action
//{
//public:
//	BuildOrRepairAction(Structure*);
//	~BuildOrRepairAction();
//
//	bool Execute(clock_t&, Unit*) override;
//	void Render() override;
//
//private:
//	Structure* target;
//};

class MoveAction : public Action
{
public:
	MoveAction(const glm::vec3&);
	~MoveAction();

	bool Execute(clock_t&, Unit*) override;

private:
	glm::vec3 m_destination;
};

//class Unit;
//class IdleDefendAction : public Action
//{
//public:
//	IdleDefendAction(float);
//	~IdleDefendAction();
//
//	bool Execute(clock_t&, Unit*) override;
//	void Render() override;
//
//private:
//	float range;
//};
class IdleAction : public Action
{
public:
	IdleAction(float durationMS);
	~IdleAction();

	bool Execute(clock_t&, Unit*) override;
private:
	float m_durationMS = 0.0f;
	clock_t m_startTime = 0;
};

class IdleChaseAttackAction : public Action
{
public:
	IdleChaseAttackAction();
	~IdleChaseAttackAction();

	bool Execute(clock_t&, Unit*) override;

};


class IdleWanderAttackAction : public Action
{
public:
	IdleWanderAttackAction();
	~IdleWanderAttackAction();

	bool Execute(clock_t&, Unit*) override;
private:
	std::list<std::shared_ptr<Action>> m_queuedActions;
};

