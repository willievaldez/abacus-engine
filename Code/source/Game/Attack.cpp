#include <Game/Attack.h>
#include <Game/Level.h>
#include <Game/Unit.h>

REGISTER_ATTACK("RangedAttack", RangedAttack);
REGISTER_ATTACK("MeleeAttack", MeleeAttack);

/*static*/
std::shared_ptr<Attack> Attack::CreateAttack(const char* attackName, Unit* owner)
{
	std::shared_ptr<Attack> returnedType = nullptr;
	auto foundType = AccessAttacks().find(attackName);
	if (foundType != AccessAttacks().end())
	{
		returnedType = foundType->second(owner);
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


bool MeleeAttack::Update()
{
	clock_t tick = clock();
	if ((tick - m_attackStart) / (float)CLOCKS_PER_SEC >= m_duration)
	{
		m_owner->SetState(State::IDLE);
		return false;
	}
	if (!m_hit)
	{
		Unit* player = Level::Get()->GetPlayerUnit();
		glm::vec3 dirToPlayer = player->GetPosition() - m_owner->GetPosition();
		dirToPlayer.z = 0.0f;
		float dist = glm::length(dirToPlayer);
		m_hit = dist < 2.5f;
		if (m_hit)
		{
			player->TakeDamage(m_dmg);
		}
	}
	return true;
}