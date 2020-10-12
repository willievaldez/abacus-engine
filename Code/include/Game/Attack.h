#pragma once

#include <GLWrapper/GLObject.h>

#include <time.h>

class Unit;

#define CONCAT_(x,y) x##y
#define CONCAT(x,y) CONCAT_(x,y)

#define REGISTER_ATTACK(TYPE_NAME, CLASS) size_t CONCAT(g_attackNum,__COUNTER__) = Attack::RegisterAttack<CLASS>(TYPE_NAME);
class Attack;
template<typename T>
std::shared_ptr<Attack> AttackFactory(Unit* owner)
{
	return std::make_shared<T>(owner);
}

enum class AttackType
{
	Melee,
	Ranged
};

class Attack : public GLObject
{
public:
	Attack(const char* assetName, Unit* owner)
		: GLObject(assetName)
		, m_owner(owner)
		, m_attackStart(clock()) {};
	virtual bool Update() = 0;
	float GetCost() const { return m_castCost; };

	template<typename T>
	static size_t RegisterAttack(const char* attackName)
	{
		auto foundType = AccessAttacks().find(attackName);
		assert(foundType == AccessAttacks().end());
		AccessAttacks()[attackName] = &AttackFactory<T>;
		return AccessAttacks().size();
	};

	static std::shared_ptr<Attack> CreateAttack(const char* attackName, Unit* owner);
	using AttackMap = std::unordered_map<std::string, std::shared_ptr<Attack>(*)(Unit*)>;

protected:
	float m_dmg = 5.0f;
	float m_castCost = 1.0f;
	AttackType m_type = AttackType::Melee;
	float m_duration = 0.0f;
	clock_t m_attackStart;
	Unit* m_owner = nullptr;

	static AttackMap& AccessAttacks() { static AttackMap attackMap; return attackMap; };
};

class RangedAttack : public Attack
{
public:
	RangedAttack(Unit* owner) : Attack("attack.png", owner) {};
	bool Update() override;
private:
	float m_speed = 0.5f;
	float m_radius = 0.5f;
};


class MeleeAttack : public Attack
{
public:
	MeleeAttack(Unit* owner) : Attack("attack.png", owner)
	{
		m_castCost = 0.0f;
		m_dmg = 10.0f;
		m_duration = 0.5f;
	};
	bool Update() override;
private:
	bool m_hit = false;
};