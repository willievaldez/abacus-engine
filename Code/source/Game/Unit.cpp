#include <Game/Unit.h>
#include <Game/Level.h>
#include <Config.h>

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

				if (key == "sprite_path")
				{
					metadata.m_spritePath = val;
				}
				else if (key == "num_sprite_rows")
				{
					metadata.m_numSpriteRows = std::stoi(val);
				}
				else if (key == "speed")
				{
					metadata.m_speed = std::stof(val);
				}
				else if (key == "health")
				{
					metadata.m_maxHealth = std::stoi(val);
				}
				else if (key == "mana_loss_per_sec")
				{
					metadata.m_manaDepletionPerSec = std::stof(val);
				}
				else if (key == "idle_action")
				{
					metadata.m_idleAction = val;
				}
				else
				{
					printf("unrecognized attribute: %s", key.c_str());
				}
			}
		}

		foundEntity = GetEntityBlueprints().emplace(std::make_pair(std::string(entityName), metadata)).first;
	}

	if (foundEntity != GetEntityBlueprints().end())
	{
		unit = new Unit(foundEntity->second);
		unit->m_unitId = Level::Get()->AddUnit(unit);
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
	m_idleAction = Action::CreateAction(m_metadata.m_idleAction.c_str());
	//friendly = isFriendly;
	//isDead = false;

	//if (friendly)
	//{
	//	idleAction = new IdleDefendAction(10.0f);
	//}
	//else
	//{
	//	idleAction = new IdleAttackAction();
	//}

}

Unit::~Unit()
{
	Level::Get()->RemoveUnit(this);

	//std::shared_ptr<Action> actionToDelete = nullptr;
	//while (m_currentAction)
	//{
	//	actionToDelete = m_currentAction;
	//	m_currentAction = m_currentAction->nextAction;
	//	delete actionToDelete;
	//}
	//delete m_idleAction;
}

void Unit::Render()
{
	if (m_currentState == State::IDLE)
	{
		UniformContainer uniforms;
		uniforms.AddObject("animationFrame", 0);
		m_asset->Render(m_position, uniforms);
	}
	else if (m_currentState == State::MOVING)
	{
		UniformContainer uniforms;
		uniforms.AddObject("animationFrame", m_animationFrame);
		m_asset->Render(m_position, uniforms);
	}
	
	m_asset->DrawStatusBar(m_position, m_currentHealth/ m_metadata.m_maxHealth);
}

void Unit::Update(clock_t tick)
{
	if (m_idleAction) m_idleAction->Execute(tick, this);

	//else if (m_currentAction->Execute(tick, this))
	//{
	//	std::shared_ptr<Action> actionToDelete = m_currentAction;
	//	m_currentAction = m_currentAction->nextAction;
	//	delete actionToDelete;
	//}

	TakeDamage(m_metadata.m_manaDepletionPerSec / GetConfig().ticksPerSecond);
}

void Unit::addAction(Action* action, bool clearActions)
{
	//if (clearActions)
	//{
	//	Action* actionToDelete = nullptr;
	//	Action* cachedAction = currentAction;
	//	while (cachedAction && cachedAction != idleAction)
	//	{
	//		actionToDelete = cachedAction;
	//		cachedAction = cachedAction->nextAction;
	//		delete actionToDelete;
	//	}
	//	idleAction->nextAction = nullptr;
	//	currentAction = nullptr;
	//}

	//if (!currentAction) currentAction = action;
	//else 
	//{
	//	Action* traversalAction = currentAction;
	//	while (traversalAction->nextAction)
	//	{
	//		traversalAction = traversalAction->nextAction;
	//	}

	//	traversalAction->nextAction = action;
	//}
}

void Unit::drawActions()
{
	//Action* actionToDraw = currentAction;
	//while (actionToDraw)
	//{
	//	actionToDraw->Render(); // TODO: figure out crash here
	//	actionToDraw = actionToDraw->nextAction;
	//}
}

void Unit::BasicAttack(const glm::vec3& origin, const glm::vec3& direction)
{

}

bool Unit::TakeDamage(float dmg)
{
	m_currentHealth -= dmg;

	if (m_currentHealth <= 0.0f)
	{
		//isDead = true;
		m_currentHealth = 0.0f;
		return true;
	}

	return false;
}

void Unit::GetMovePosition(const glm::vec3& direction, glm::vec3& destinationOut)
{
	clock_t now = clock();
	if ((now - m_lastFrameTick) / (float)CLOCKS_PER_SEC > 0.043f)
	{
		m_animationFrame++;
		if (m_animationFrame >= m_asset->GetNumFrames())
		{
			m_animationFrame = 1;
		}
		m_lastFrameTick = now;
	}

	destinationOut = m_position + (direction * m_metadata.m_speed / (float)GetConfig().ticksPerSecond);
}