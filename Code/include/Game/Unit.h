#pragma once

#include <GLWrapper/GLObject.h>
#include <Game/Action.h>

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
	std::string m_spritePath = "";
	int m_numSpriteRows = 1;
	int m_maxHealth = 100;
	float m_speed = 4.0f;
	float m_manaDepletionPerSec = 0.0f;
	std::string m_idleAction = "";
	float m_dodgeSpeed = 16.0f;
	float m_dodgeDurationSec = 0.25f;
	float m_dodgeCost = 10.0f;
	float m_atkSpeed = 0.5f;
	float m_radius = 1.5f;
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

	void BasicAttack(const glm::vec3&, const glm::vec3&);
	bool TakeDamage(float);
	float GetHealth() const { return m_currentHealth; };
	void GetMovePosition(const::glm::vec3& direction, glm::vec3& destination);
	void StartDodge();
private:
	Unit(const UnitMetadata&);

	using EntityMap = std::unordered_map<std::string, UnitMetadata>;
	static EntityMap& GetEntityBlueprints() {
		static EntityMap entityMap;
		return entityMap;
	};
	UnitMetadata m_metadata;

	State m_currentState = State::IDLE;
	std::shared_ptr<Action> m_idleAction = nullptr;
	float m_currentHealth = 0.0f;
	int m_unitId = -1;
	clock_t m_lastFrameTick;
	int m_animationFrame = 0;
	clock_t m_dodgeStartTime;
	clock_t m_lastAttack;
	std::vector<Attack*> m_activeAttacks;
};