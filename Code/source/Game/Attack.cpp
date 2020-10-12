#include <Game/Attack.h>
#include <Game/Level.h>
#include <Game/Unit.h>
#include <Config.h>

#include <fstream>
#include <sstream>

REGISTER_ATTACK("Ranged", RangedAttack);
REGISTER_ATTACK("Melee", MeleeAttack);

/*static*/
std::shared_ptr<Attack> Attack::Create(const char* attackName, Unit* owner)
{
	std::shared_ptr<Attack> returnedType = nullptr;

	AttackMetadata metadata;
	auto foundAttack = GetAttackBlueprints().find(attackName);
	if (foundAttack == GetAttackBlueprints().end())
	{
		// get expected attributes
		AttributeContainer expectedAttributes = metadata.GetExpectedAttributes();

		std::string line;
		std::ifstream myfile(INSTALL_DIR + "Assets/2D/Attacks/" + attackName + ".atk");
		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				std::stringstream lineStream(line);
				std::string key, val;
				getline(lineStream, key, '=');
				getline(lineStream, val, '=');
				expectedAttributes.SetAttribute(key, val);
			}
		}

		foundAttack = GetAttackBlueprints().emplace(std::make_pair(std::string(attackName), metadata)).first;
	}

	if (foundAttack != GetAttackBlueprints().end())
	{
		auto foundType = AccessAttacks().find(foundAttack->second.m_type);
		if (foundType != AccessAttacks().end())
		{
			returnedType = foundType->second(owner, foundAttack->second);
		}
	}
	else
	{
		printf("Attack Blueprint(atk) not found: %s\n", attackName);
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
				hitUnit->TakeDamage(m_metadata.m_dmg);
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
	if ((tick - m_attackStart) / (float)CLOCKS_PER_SEC >= m_metadata.m_castTime)
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
			player->TakeDamage(m_metadata.m_dmg);
		}
	}
	return true;
}
