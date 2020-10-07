#include <Game/Unit.h>
#include <Game/Level.h>
#include <Config.h>

#include <glm/gtc/matrix_transform.hpp> // translate

Unit* Unit::Create(const char* asset)
{
	return Create(GLObject::GLAsset(asset));
}

Unit* Unit::Create(Asset* asset)
{
	Unit* unit = new Unit(asset);
	unit->m_unitId = Level::Get()->AddUnit(unit);
	return unit;
}

Unit::Unit(Asset* asset) : GLObject(asset)
{
	OBJECT_TYPE = ObjectType::UNIT;
	m_currentHealth = (float)m_maxHealth;
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

	Action* actionToDelete = nullptr;
	while (currentAction)
	{
		actionToDelete = currentAction;
		currentAction = currentAction->nextAction;
		delete actionToDelete;
	}
	delete idleAction;
}

void Unit::Render()
{
	if (m_currentState == State::IDLE)
	{
		m_asset->Render(m_position, 0);
	}
	else if (m_currentState == State::MOVING)
	{
		m_asset->Render(m_position, m_animationFrame);
	}
	
	m_asset->DrawStatusBar(m_position, m_currentHealth/m_maxHealth);
}

void Unit::Update(clock_t tick)
{
	//if (!currentAction) idleAction->execute(tick, this);

	//else if (currentAction->execute(tick, this))
	//{
	//	Action* actionToDelete = currentAction;
	//	currentAction = currentAction->nextAction;
	//	delete actionToDelete;
	//}

	TakeDamage(m_manaDepletionPerSec / GetConfig().ticksPerSecond);
}

void Unit::addAction(Action* action, bool clearActions)
{
	if (clearActions)
	{
		Action* actionToDelete = nullptr;
		Action* cachedAction = currentAction;
		while (cachedAction && cachedAction != idleAction)
		{
			actionToDelete = cachedAction;
			cachedAction = cachedAction->nextAction;
			delete actionToDelete;
		}
		idleAction->nextAction = nullptr;
		currentAction = nullptr;
	}

	if (!currentAction) currentAction = action;
	else 
	{
		Action* traversalAction = currentAction;
		while (traversalAction->nextAction)
		{
			traversalAction = traversalAction->nextAction;
		}

		traversalAction->nextAction = action;
	}
}

void Unit::drawActions()
{
	Action* actionToDraw = currentAction;
	while (actionToDraw)
	{
		actionToDraw->draw(); // TODO: figure out crash here
		actionToDraw = actionToDraw->nextAction;
	}
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

	destinationOut = m_position + (direction * m_speed / (float)GetConfig().ticksPerSecond);
}