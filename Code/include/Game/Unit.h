#pragma once

#include <GLWrapper/GLObject.h>
#include <Game/Action.h>

#include <chrono>

class Action;

class Unit : public GLObject
{
public:
	static Unit* Create(const char* entityName);
	~Unit();

	void Render() override;
	void Update(clock_t);

	void BasicAttack(const glm::vec3&, const glm::vec3&);
	bool TakeDamage(float);
	void addAction(Action*, bool);
	void drawActions();
	float GetHealth() { return m_currentHealth; };
	void GetMovePosition(const::glm::vec3& direction, glm::vec3& destination);
private:
	struct UnitMetadata
	{
		std::string m_spritePath = "";
		int m_numSpriteRows = 1;
		int m_maxHealth = 100;
		float m_speed = 4.0f;
		float m_manaDepletionPerSec = 0.0f;
		std::string m_idleAction = "";
	};

	Unit(const UnitMetadata&);

	using EntityMap = std::unordered_map<std::string, UnitMetadata>;
	static EntityMap& GetEntityBlueprints() {
		static EntityMap entityMap;
		return entityMap;
	};
	UnitMetadata m_metadata;

	std::shared_ptr<Action> m_idleAction = nullptr;
	float m_currentHealth = 0.0f;
	int m_unitId = -1;
	clock_t m_lastFrameTick;
	int m_animationFrame = 0;
};