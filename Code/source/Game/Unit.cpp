#include <Game/Unit.h>
#include <Game/Level.h>
#include <Utility/Config.h>
#include <Game/Attack.h>
#include <Game/Item.h>

#include <fstream>
#include <sstream>

static const int ticksPerSecond = GetConfig("Shared").ticksPerSecond;

Unit* Unit::Create(const char* entityName)
{
	Unit* unit = nullptr;
	UnitMetadata metadata;
	auto foundEntity = GetEntityBlueprints().find(entityName);
	if (foundEntity == GetEntityBlueprints().end())
	{
		// get expected attributes
		AttributeContainer expectedAttributes = metadata.GetExpectedAttributes();

		std::string line;
		std::ifstream myfile(INSTALL_DIR + "Assets/2D/Entities/" + entityName + ".ntt");
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

		foundEntity = GetEntityBlueprints().emplace(std::make_pair(std::string(entityName), metadata)).first;
	}

	if (foundEntity != GetEntityBlueprints().end())
	{
		unit = new Unit(foundEntity->second);
		unit->m_unitId = Level::Get()->AddUnit(unit);
	}
	else
	{
		printf("Entity not found: %s\n", entityName);
	}

	return unit;
};

Unit::Unit(const UnitMetadata& metadata)
	: GLObject(GLObject::GLAsset(metadata.sprite_path.c_str()))
	, m_metadata(metadata)
{
	OBJECT_TYPE = ObjectType::UNIT;
	m_currentHealth = (float)m_metadata.health;
	m_lastFrameTick = clock();
	m_dodgeStartTime = 0;
	m_lastAttack = 0;
	m_idleAction = Action::CreateAction(m_metadata.idle_action.c_str());
}

Unit::~Unit()
{
	Item::Create("mana_orb.png", GetPosition());
	Level::Get()->RemoveUnit(this);
}

void Unit::Render()
{
	if (m_currentState == State::IDLE)
	{
		UniformContainer uniforms;
		uniforms.AddObject("animationFrame", 0);
		m_asset->Render(m_position, uniforms);
	}
	else if (m_currentState == State::MOVING || m_currentState == State::DODGING || m_currentState == State::ATTACKING)
	{
		UniformContainer uniforms;
		uniforms.AddObject("animationFrame", m_animationFrame);
		m_asset->Render(m_position, uniforms);
	}

	for (auto& attack : m_activeAttacks)
	{
		attack->Render();
	}

	m_asset->DrawStatusBar(m_position, m_currentHealth / m_metadata.health);
}

void Unit::Update(clock_t tick)
{
	if (m_idleAction) m_idleAction->Execute(tick, this);

	auto attackIterator = m_activeAttacks.begin();
	while (attackIterator != m_activeAttacks.end())
	{
		std::shared_ptr<Attack> attack = *attackIterator;
		attack->Update();
		if (attack->GetState() == Attack::State::DONE) // attack ended
		{
			// delete the object (shared_ptr will delete it for us when ref cout <1)
			attackIterator = m_activeAttacks.erase(attackIterator); // advance the iterator
		}
		else
		{
			attackIterator++;
		}
	}

	TakeDamage(m_metadata.mana_loss_per_sec / ticksPerSecond);
}

void Unit::SetState(State state)
{
	m_currentState = state;
}

const Unit::State& Unit::GetState() const
{
	return m_currentState;
}

const UnitMetadata& Unit::GetMetadata() const
{
	return m_metadata;
}

void Unit::BasicAttack(const glm::vec3& direction)
{
	static int attackNum = 0;
	clock_t tick = clock();
	if ((tick - m_lastAttack) / (float)CLOCKS_PER_SEC >= m_metadata.attack_cooldown)
	{
		SetState(State::ATTACKING);
		m_lastAttack = tick;
		std::shared_ptr<Attack> attack = Attack::Create(m_metadata.basic_attack.c_str(), this);
		attack->SetPosition(GetPosition());
		attack->SetDirection(direction);
		m_activeAttacks.push_back(attack);
		TakeDamage(attack->GetCost());
	}
}

bool Unit::TakeDamage(float dmg)
{
	if (m_currentState != State::DODGING || dmg < 0.0f)
	{
		m_currentHealth -= dmg;

		if (m_currentHealth <= 0.0f)
		{
			m_currentHealth = 0.0f;
			return true;
		}
		if (m_currentHealth > m_metadata.health)
		{
			m_currentHealth = (float)m_metadata.health;
		}
	}

	return false;
}

void Unit::MoveToNextPosition(const clock_t& tick)
{
	glm::vec3 destination(m_position);
	if (m_currentState == State::MOVING || m_currentState == State::DODGING)
	{
		// TODO: only update animation frame if actually moving
		if ((tick - m_lastFrameTick) / (float)CLOCKS_PER_SEC > 0.043f)
		{
			m_animationFrame++;
			if (m_animationFrame >= m_asset->GetNumFrames())
			{
				m_animationFrame = 1;
			}
			m_lastFrameTick = tick;
		}
	}
	if (m_currentState == State::MOVING)
	{
		destination = m_position + (m_direction * m_metadata.speed / (float)ticksPerSecond);
	}
	else if (m_currentState == State::DODGING)
	{
		if ((tick - m_dodgeStartTime) / (float)CLOCKS_PER_SEC < m_metadata.dodge_duration)
		{
			destination = m_position + (m_direction * m_metadata.dodge_speed / (float)ticksPerSecond);
		}
		else // dodge ended
		{
			m_currentState = State::IDLE;
		}
	}

	Tile* destTile = Level::Get()->GetTileFromCoords(destination);

	if (destTile)
	{
		if (destTile->Collision(destination))
		{
			Tile* srcTile = Level::Get()->GetTileFromCoords(GetPosition());
			glm::vec3 tileChangeDirection = destTile->GetPosition() - srcTile->GetPosition();
			destTile = nullptr; // reset dest tile

			if (tileChangeDirection.x)
			{
				glm::vec3 newDest = destination;
				newDest.x = GetPosition().x;
				destTile = Level::Get()->GetTileFromCoords(newDest);

				if (destTile && !destTile->Collision(newDest))
				{
					destination = newDest;
				}
				else
				{
					destTile = nullptr;
				}
			}
			if (!destTile && tileChangeDirection.y)
			{
				glm::vec3 newDest = destination;
				newDest.y = GetPosition().y;
				destTile = Level::Get()->GetTileFromCoords(newDest);

				if (destTile && !destTile->Collision(destination))
				{
					destination = newDest;
				}
				else
				{
					destTile = nullptr;
				}
			}
		}

		if (destTile)
		{
			SetPosition(destination);
			destTile->Interact(this);
		}
	}
	else
	{
		SetState(State::IDLE);
	}
}

void Unit::StartDodge()
{
	m_dodgeStartTime = clock();
	TakeDamage(m_metadata.dodge_cost);
	SetState(State::DODGING);
}