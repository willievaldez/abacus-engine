#include <Game/Attack.h>
#include <Game/Level.h>
#include <Game/Unit.h>
#include <Utility/Config.h>
#include <GLWrapper/PointLight.h>

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
		auto foundType = AccessAttacks().find(foundAttack->second.type);
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

RangedAttack::RangedAttack(Unit* owner, const AttackMetadata& metadata) : Attack(owner, metadata)
{
	m_light = PointLight::Create(glm::vec3(0.0f), 1.0f, metadata.radius);
}

void RangedAttack::SetPosition(const glm::vec3& pos)
{
	m_light->pos = pos;
	Attack::SetPosition(pos);
}

RangedAttack::~RangedAttack()
{
	if (m_light)
	{
		delete m_light;
	}
}

bool RangedAttack::Update()
{
	if (m_owner->GetState() == State::ATTACKING)
	{
		clock_t tick = clock();
		if ((tick - m_attackStart) / (float)CLOCKS_PER_SEC >= m_metadata.cast_time)
		{
			m_owner->SetState(State::IDLE);
		}
	}

	bool keepGoing = true;
	std::set<Unit*> hitUnits;
	std::vector<Tile*> tiles = Level::Get()->GetTilesFromCoords(GetPosition(), m_metadata.radius);
	for (auto& tile : tiles)
	{
		keepGoing = keepGoing && !tile->Collision(GetPosition(), hitUnits, m_metadata.radius);
	}

	for (auto& hitUnit : hitUnits)
	{
		if (hitUnit != m_owner)
		{
			hitUnit->TakeDamage(m_metadata.damage);
			keepGoing = false;
		}
	}

	// no collision, keep moving
	glm::vec3 newAttackPos = GetPosition() + (GetDirection() * m_metadata.speed);
	SetPosition(newAttackPos);
	return keepGoing;
}


bool MeleeAttack::Update()
{
	clock_t tick = clock();
	if ((tick - m_attackStart) / (float)CLOCKS_PER_SEC >= m_metadata.cast_time)
	{
		m_owner->SetState(State::IDLE);
		return false;
	}
	if (!m_hit)
	{
		std::set<Unit*> hitUnits;
		float hitDistance = m_owner->GetMetadata().hitbox_radius + m_metadata.radius;
		std::vector<Tile*> tiles = Level::Get()->GetTilesFromCoords(GetPosition(), hitDistance);
		for (auto& tile : tiles)
		{
			tile->Collision(GetPosition(), hitUnits, hitDistance);
		}

		for (auto& hitUnit : hitUnits)
		{
			if (hitUnit != m_owner)
			{
				hitUnit->TakeDamage(m_metadata.damage);
				m_hit = true;
			}
		}
	}
	return true;
}
