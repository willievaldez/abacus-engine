#pragma once

#include <GLWrapper/GLObject.h>
#include <Utility/Utility.h>
#include <Utility/AttributeContainer.h>

#include <time.h>
#include <vector>

class Unit;
class Attack;
class Tile;
class PointLight;

#define REGISTER_ATTACK(TYPE_NAME, CLASS) REGISTER(Attack, TYPE_NAME, CLASS)

struct AttackMetadata
{
#define ATTRIBUTE(type, varName, defaultVal) type varName = defaultVal;
#include <Game/AttackMetadata.inl>
#undef ATTRIBUTE

	AttributeContainer GetExpectedAttributes()
	{
		AttributeContainer attributeContainer;
#define ATTRIBUTE(type, varName, defaultVal) attributeContainer.AddAttribute(#varName, &varName)
#include <Game/AttackMetadata.inl>
#undef ATTRIBUTE
		return attributeContainer;
	};
};

template<typename T>
std::shared_ptr<Attack> AttackFactory(Unit* owner, const AttackMetadata& metadata)
{
	return std::make_shared<T>(owner, metadata);
}

class Attack : public GLObject
{
public:
	enum class State
	{
		CHANNELING,
		FIRING,
		FADING,
		DONE
	};
	virtual void Update() = 0;
	State GetState() { return m_state; };
	float GetCost() const { return m_metadata.cast_cost; };
	Unit* GetAttachedUnit() { return m_attachedUnit; };

	template<typename T>
	static size_t Register(const char* attackName)
	{
		auto foundType = AccessAttacks().find(attackName);
		assert(foundType == AccessAttacks().end());
		AccessAttacks()[attackName] = &AttackFactory<T>;
		return AccessAttacks().size();
	};

	static std::shared_ptr<Attack> Create(const char* attackName, Unit* owner);
	using AttackClassMap = std::unordered_map<std::string, std::shared_ptr<Attack>(*)(Unit*, const AttackMetadata&)>;

protected:
	Attack(Unit* owner, const AttackMetadata& metadata)
		: GLObject(metadata.sprite.c_str())
		, m_metadata(metadata)
		, m_owner(owner)
		, m_stateStart(clock()) {};

	using AttackBPMap = std::unordered_map<std::string, AttackMetadata>;
	static AttackBPMap& GetAttackBlueprints() { static AttackBPMap attackBPMap; return attackBPMap; };
	AttackMetadata m_metadata;

	Unit* m_owner = nullptr;
	Unit* m_attachedUnit = nullptr;
	glm::vec3 m_attachmentOffset;
	clock_t m_stateStart;
	State m_state = State::CHANNELING;
	void SetState(State);
	std::vector<Tile*> m_tilesAffected;

	static AttackClassMap& AccessAttacks() { static AttackClassMap attackClassMap; return attackClassMap; };
};

class RangedAttack : public Attack
{
public:

	RangedAttack(Unit* owner, const AttackMetadata& metadata);
	~RangedAttack();
	void Update() override;
	void Render() override;

	void SetPosition(const glm::vec3&) override;
private:
	PointLight* m_light = nullptr;
};

class MeleeAttack : public Attack
{
public:
	MeleeAttack(Unit* owner, const AttackMetadata& metadata) : Attack(owner, metadata) {};
	void Update() override;
private:
	bool m_hit = false;
};