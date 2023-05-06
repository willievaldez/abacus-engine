#include <Game/Unit.h>
#include <Game/Level.h>
#include <Utility/Config.h>
#include <Game/Attack.h>
#include <Game/Item.h>

#include <fstream>
#include <sstream>

namespace
{
	const Config& config = GetConfig("Shared");
	static const int ticksPerSecond = config.ticksPerSecond;
	static const float unitRenderOffset = config.unitRenderOffset;
}

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
	// shift render position up by the specified
	glm::vec3 renderPosition = m_position;
	renderPosition.y += unitRenderOffset;

	if (m_currentState == State::IDLE)
	{
		UniformContainer uniforms;
		uniforms.AddObject("animationFrame", 0);
		m_asset->Render(renderPosition, uniforms);
	}
	else if (m_currentState == State::MOVING || m_currentState == State::DODGING || m_currentState == State::ATTACKING)
	{
		UniformContainer uniforms;
		uniforms.AddObject("animationFrame", m_animationFrame);
		m_asset->Render(renderPosition, uniforms);
	}

	for (auto& attack : m_activeAttacks)
	{
		attack->Render();
	}

	m_asset->DrawStatusBar(renderPosition, m_currentHealth / m_metadata.health);
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
			destination = m_position + (m_direction * (m_metadata.dodge_dist / m_metadata.dodge_duration) / (float)ticksPerSecond);
		}
		else // dodge ended
		{
			//printf("dest: %f, %f, %f\n", m_position.x, m_position.y, m_position.z);
			SetPosition(m_dodgeDestination);
			SetState(State::IDLE);
			return;
		}
	}

	Tile* destTile = Level::Get()->GetTileFromCoords(destination);
	if (destTile)
	{
		Tile::TraversalType traversalType = m_metadata.friendly ? Tile::TraversalType::Friendly : Tile::TraversalType::Enemy;
		if (!destTile->Traversable(traversalType))
		{
			Tile* srcTile = Level::Get()->GetTileFromCoords(GetPosition());
			glm::vec3 tileChangeDirection = destTile->GetPosition() - srcTile->GetPosition();
			destTile = nullptr; // reset dest tile
			printf("dest is not traversable: %f, %f, %f\n", destination.x, destination.y, destination.z);

			if (tileChangeDirection.x)
			{
				glm::vec3 newDest = destination;
				newDest.x = GetPosition().x;
				destTile = Level::Get()->GetTileFromCoords(newDest);
				if (destTile && destTile->Traversable(traversalType))
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

				if (destTile && destTile->Traversable(traversalType))
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

bool Unit::StartDodge(const glm::vec3& destination, const clock_t& tick)
{
	if (GetState() == State::DODGING)
	{
		return false;
	}

	m_dodgeStartTime = tick;
	m_dodgeDestination = destination;
	SetDirection(m_dodgeDestination - GetPosition());
	TakeDamage(m_metadata.dodge_cost);
	SetState(State::DODGING);

	return true;

}

void Unit::ProcessInput(const KeyMap& rawKeyMap, const clock_t& tick)
{
	// if returning to idle, process input buffer first
	const KeyMap* keyMapPtr = &rawKeyMap;
	bool inputBufferUsed = false;
	if (GetState() == State::IDLE && !m_inputBuffer.empty())
	{
		//printf("using input buffer for this tick\n");
		keyMapPtr = &m_inputBuffer.front();
		inputBufferUsed = true;
	}

	// get player direction
	glm::vec3 direction(0.0f);
	if (!keyMapPtr->IsNull())
	{
		if ((*keyMapPtr)[GLFW_KEY_W] || (*keyMapPtr)[GLFW_KEY_UP])
		{
			direction.y += config.tileSize;
		}
		else if ((*keyMapPtr)[GLFW_KEY_A] || (*keyMapPtr)[GLFW_KEY_LEFT])
		{
			direction.x -= config.tileSize;
		}
		else if ((*keyMapPtr)[GLFW_KEY_S] || (*keyMapPtr)[GLFW_KEY_DOWN])
		{
			direction.y -= config.tileSize;
		}
		else if ((*keyMapPtr)[GLFW_KEY_D] || (*keyMapPtr)[GLFW_KEY_RIGHT])
		{
			direction.x += config.tileSize;
		}
	}

	bool validInput = !rawKeyMap.IsNull() || inputBufferUsed;
	if (validInput && !inputBufferUsed)
	{
		// destination is invalid, discard input
		glm::vec3 destPosition = m_position + direction;
		Tile::TraversalType traversalType = m_metadata.friendly ? Tile::TraversalType::Friendly : Tile::TraversalType::Enemy;

		Tile* destTile = Level::Get()->GetTileFromCoords(destPosition);
		if (!destTile || !destTile->Traversable(traversalType))
		{
			printf("dest position invalid, discarding input: current pos: (%f, %f, %f), dest pos: (%f, %f, %f)\n",
				m_position.x, m_position.y, m_position.z, destPosition.x, destPosition.y, destPosition.z);
			validInput = false;
		}
	}

	// if not idle, check if we should add the new input to the buffer
	bool inputBuffered = false;
	if (validInput && GetState() != State::IDLE && m_inputBuffer.size() < config.inputBufferSize)
	{
		float msSpentInCurrentAction = ((tick - m_dodgeStartTime) * 1000 / (float)CLOCKS_PER_SEC);
		float msUntilCurrentActionEnds = (m_metadata.dodge_duration * 1000) - msSpentInCurrentAction;
		//printf("duration: %f, ms spent: %f, ms remaining: %f\n", m_metadata.dodge_duration, msSpentInCurrentAction, msUntilCurrentActionEnds);
		if (msUntilCurrentActionEnds < config.inputBufferWindowMS)
		{
			//printf("input buffered\n");
			m_inputBuffer.push_back(rawKeyMap);
			inputBuffered = true;
		}
	}

	if (!inputBuffered && validInput)
	{
		printf("starting dodge. current pos: (%f, %f, %f), direction: (%f, %f, %f)\n",
			m_position.x, m_position.y, m_position.z, direction.x, direction.y, direction.z);
		StartDodge(m_position+direction, tick);
	}

	if (GetState() == Unit::State::DODGING || GetState() == Unit::State::MOVING)
	{
		MoveToNextPosition(tick);
	}

	// pop from input buffer if necessary
	if (inputBufferUsed)
	{
		m_inputBuffer.pop_front();
	}
}