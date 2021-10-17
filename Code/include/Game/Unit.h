#pragma once

#include <GLWrapper/GLObject.h>
#include <Game/Action.h>
#include <Utility/AttributeContainer.h>

#include <chrono>

enum class State
{
	IDLE,
	DODGING,
	MOVING,
	ATTACKING
};

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
	glm::vec3 GetNextPosition();
	void StartDodge();
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
	clock_t m_lastAttack;
	std::vector<std::shared_ptr<Attack>> m_activeAttacks;
};