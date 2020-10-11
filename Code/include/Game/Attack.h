#pragma once

#include <GLWrapper/GLObject.h>

#define REGISTER_ATTACK(TYPE_NAME, CLASS) size_t g_attackNum##__COUNTER__ = Attack::RegisterAttack<CLASS>(TYPE_NAME);
class Attack;
template<typename T>
std::shared_ptr<Attack> AttackFactory()
{
	return std::make_shared<T>();
}

class Attack : public GLObject
{
public:
	Attack(const char* assetName) : GLObject(assetName) {};
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

	static std::shared_ptr<Attack> CreateAttack(const char* attackName);
	using AttackMap = std::unordered_map<std::string, std::shared_ptr<Attack>(*)()>;

protected:
	float m_dmg = 5.0f;
	float m_castCost = 1.0f;

	static AttackMap& AccessAttacks() { static AttackMap attackMap; return attackMap; };
};

class RangedAttack : public Attack
{
public:
	RangedAttack() : Attack("attack.png") {};
	bool Update() override;
private:
	float m_speed = 0.3f;
	float m_radius = 0.5f;
};