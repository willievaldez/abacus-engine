#pragma once

#include <Game/Level.h>

#include <chrono>

#define REGISTER_ACTION(TYPE_NAME, CLASS) size_t g_actionNum##__COUNTER__ = Action::RegisterAction<CLASS>(TYPE_NAME);
class Action;
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
	virtual void Render() = 0;

	template<typename T>
	static size_t RegisterAction(const char* actionName)
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
//
//class MoveAction : public Action
//{
//public:
//	MoveAction(glm::vec3&);
//	~MoveAction();
//
//	bool Execute(clock_t&, Unit*) override;
//	void Render() override;
//
//private:
//	glm::vec3 destination;
//};
//
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

class IdleAttackAction : public Action
{
public:
	IdleAttackAction();
	~IdleAttackAction();

	bool Execute(clock_t&, Unit*) override;
	void Render() override;

};


