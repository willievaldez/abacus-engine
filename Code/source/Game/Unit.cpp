#include <Game/Unit.h>
#include <Game/Level.h>
#include <Config.h>
#include <Game/Attack.h>

#include <fstream>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp> // translate

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
	: GLObject(GLObject::GLAsset(metadata.m_spritePath.c_str()))
	, m_metadata(metadata)
{
	OBJECT_TYPE = ObjectType::UNIT;
	m_currentHealth = (float)m_metadata.m_maxHealth;
	m_lastFrameTick = clock();
	m_dodgeStartTime = m_lastFrameTick;
	m_lastAttack = m_lastFrameTick;
	m_idleAction = Action::CreateAction(m_metadata.m_idleAction.c_str());
}

Unit::~Unit()
{
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

	m_asset->DrawStatusBar(m_position, m_currentHealth / m_metadata.m_maxHealth);
}

void Unit::Update(clock_t tick)
{
	if (m_idleAction) m_idleAction->Execute(tick, this);

	auto attackIterator = m_activeAttacks.begin();
	while (attackIterator != m_activeAttacks.end())
	{
		std::shared_ptr<Attack> attack = *attackIterator;
		if (!attack->Update()) // attack ended
		{
			// delete the object (shared_ptr will delete it for us when ref cout <1)
			attackIterator = m_activeAttacks.erase(attackIterator); // advance the iterator
		}
		else
		{
			attackIterator++;
		}
	}

	TakeDamage(m_metadata.m_manaDepletionPerSec / GetConfig().ticksPerSecond);
}

void Unit::SetState(State state)
{
	m_currentState = state;
}

const State& Unit::GetState() const
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
	if ((tick - m_lastAttack) / (float)CLOCKS_PER_SEC >= m_metadata.m_atkSpeed)
	{
		SetState(State::ATTACKING);
		// TODO: cannot shoot while standing next to wall
		m_lastAttack = tick;
		std::shared_ptr<Attack> attack = Attack::Create(m_metadata.m_basicAttack.c_str(), this);
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
			//isDead = true;
			m_currentHealth = 0.0f;
			return true;
		}
		if (m_currentHealth > m_metadata.m_maxHealth)
		{
			m_currentHealth = (float)m_metadata.m_maxHealth;
		}
	}

	return false;
}

void Unit::GetMovePosition(const glm::vec3& direction, glm::vec3& destinationOut)
{
	clock_t tick = clock();
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
		destinationOut = m_position + (direction * m_metadata.m_speed / (float)GetConfig().ticksPerSecond);
	}
	else if (m_currentState == State::DODGING)
	{
		if ((tick - m_dodgeStartTime) / (float)CLOCKS_PER_SEC < m_metadata.m_dodgeDurationSec)
		{
			destinationOut = m_position + (m_direction * m_metadata.m_dodgeSpeed / (float)GetConfig().ticksPerSecond);
		}
		else // dodge ended
		{
			m_currentState = State::IDLE;
		}
	}

}

void Unit::StartDodge()
{
	m_dodgeStartTime = clock();
	TakeDamage(m_metadata.m_dodgeCost);
	SetState(State::DODGING);
}