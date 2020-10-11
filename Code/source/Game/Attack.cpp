#include <Game/Attack.h>
#include <Game/Level.h>

REGISTER_ATTACK("RangedAttack", RangedAttack);

/*static*/
std::shared_ptr<Attack> Attack::CreateAttack(const char* attackName)
{
	std::shared_ptr<Attack> returnedType = nullptr;
	auto foundType = AccessAttacks().find(attackName);
	if (foundType != AccessAttacks().end())
	{
		returnedType = foundType->second();
	}
	return returnedType;
}

bool RangedAttack::Update()
{
	std::vector<Tile*> tiles = Level::Get()->GetTilesFromCoords(GetPosition(), m_radius);
	for (auto& tile : tiles)
	{
		Unit* hitUnit = nullptr;
		if (tile && tile->Collision(GetPosition(), &hitUnit, m_radius))
		{
			if (hitUnit)
			{
				hitUnit->TakeDamage(m_dmg);
			}

			return false;
		}
	}
	if (tiles.size() == 0) return false;

	// no collision, keep moving
	glm::vec3 newAttackPos = GetPosition() + (GetDirection() * m_speed);
	SetPosition(newAttackPos);
	return true;
}