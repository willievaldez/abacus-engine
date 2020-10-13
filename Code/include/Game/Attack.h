#pragma once

#include <GLWrapper/GLObject.h>
#include <Utility.h>
#include <Game/AttributeContainer.h>

#include <time.h>

class Unit;
class Attack;

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
	virtual bool Update() = 0;
	float GetCost() const { return m_metadata.cast_cost; };

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
		, m_attackStart(clock()) {};

	using AttackBPMap = std::unordered_map<std::string, AttackMetadata>;
	static AttackBPMap& GetAttackBlueprints() { static AttackBPMap attackBPMap; return attackBPMap; };
	AttackMetadata m_metadata;

	Unit* m_owner = nullptr;
	clock_t m_attackStart;

	static AttackClassMap& AccessAttacks() { static AttackClassMap attackClassMap; return attackClassMap; };
};

class RangedAttack : public Attack
{
public:
	RangedAttack(Unit* owner, const AttackMetadata& metadata) : Attack(owner, metadata) {};
	bool Update() override;
private:
};

class MeleeAttack : public Attack
{
public:
	MeleeAttack(Unit* owner, const AttackMetadata& metadata) : Attack(owner, metadata) {};
	bool Update() override;
private:
	bool m_hit = false;
};