#pragma once

#include <GLWrapper/GLObject.h>
#include <Game/Action.h>
#include <Utility/AttributeContainer.h>
#include <Utility/KeyMap.h>

#include <chrono>
#include <list>

class Action; // lawsuit
class Attack;

struct UnitMetadata
{
#define ATTRIBUTE(type, varName, defaultVal) type varName = defaultVal;
#include <Game/UnitMetadata.inl>
#undef ATTRIBUTE

	AttributeContainer GetExpectedAttributes()
	{
		AttributeContainer attributeContainer;
#define ATTRIBUTE(type, varName, defaultVal) attributeContainer.AddAttribute(#varName, &varName)
#include <Game/UnitMetadata.inl>
#undef ATTRIBUTE
		return attributeContainer;
	};
};

class Unit : public GLObject
{
public:
	enum class State
	{
		IDLE,
		DODGING,
		MOVING,
		ATTACKING
	};

	static Unit* Create(const char* entityName);
	~Unit();

	void Render() override;
	void Update(clock_t);

	const State& GetState() const;
	void SetState(State);
	const UnitMetadata& GetMetadata() const;

	void BasicAttack(const glm::vec3& direction);
	bool TakeDamage(float);
	float GetHealth() const { return m_currentHealth; };
	void MoveToNextPosition(const clock_t& tick);
	bool StartDodge(const glm::vec3& direction, const clock_t& tick);
	void ProcessInput(const KeyMap& keyMap, const clock_t& tick);
private:
	Unit(const UnitMetadata&);

	using EntityMap = std::unordered_map<std::string, UnitMetadata>;
	static EntityMap& GetEntityBlueprints() { static EntityMap entityMap; return entityMap; };
	UnitMetadata m_metadata;

	State m_currentState = State::IDLE;
	std::shared_ptr<Action> m_idleAction = nullptr;
	float m_currentHealth = 0.0f;
	int m_unitId = -1;
	clock_t m_lastFrameTick;
	int m_animationFrame = 0;
	clock_t m_dodgeStartTime;
	glm::vec3 m_dodgeDestination;
	clock_t m_lastAttack;
	std::vector<std::shared_ptr<Attack>> m_activeAttacks;
	std::list<KeyMap> m_inputBuffer;
};