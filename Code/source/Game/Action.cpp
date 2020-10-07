#include <Game/Action.h>

Action::Action()
{
	type = ActionType::Idle;
	lastUpdateInterval = clock();
	nextAction = nullptr;
}

Action::~Action()
{

}

// TARGET ACTION ------------------------------------------------

TargetAction::TargetAction(Unit* target)
{
	type = ActionType::Target;
	this->target = target;
}

TargetAction::~TargetAction()
{

}

bool TargetAction::execute(clock_t& tick, Unit* unit)
{
	//glm::vec3 simplePath = target->GetPosition() - unit->GetPosition();
	//Tile* sourceTile = level->GetTileFromCoords(unit->GetPosition());
	//Tile* destTile = level->GetTileFromCoords(target->GetPosition());
	//if (!target->isDead
	//	&& sourceTile
	//	&& destTile
	//	&& !destTile->Collision())
	//{
	//	glm::vec3 newPosition;
	//	if (glm::length(simplePath) > 1.0f)
	//	{
	//		newPosition = unit->GetPosition() + (glm::normalize(simplePath) / 10.0f);
	//		unit->SetPosition(newPosition);
	//		unit->setState(State::MOVING);
	//	}
	//	if (glm::length(simplePath) < 1.5f)
	//	{
	//		target->TakeDamage(0.2f);
	//		if (!target->isDead)
	//		{
	//			unit->setState(State::ATTACKING);
	//			return true;
	//		}
	//	}
	//}

	unit->setState(State::IDLE);
	return false;
}

void TargetAction::draw()
{
	GLObject::GLAsset("redtarget.png")->Render(target->GetPosition());
}

// MOVE ACTION ------------------------------------------------

MoveAction::MoveAction(glm::vec3& dest)
{
	type = ActionType::Move;
	destination = dest;
}

MoveAction::~MoveAction()
{

}

bool MoveAction::execute(clock_t& tick, Unit* unit)
{
	//glm::vec3 simplePath = destination - unit->GetPosition();
	//Tile* sourceTile = level->GetTileFromCoords(unit->GetPosition());
	//Tile* destTile = level->GetTileFromCoords(simplePath);
	//if (sourceTile
	//	&& destTile
	//	&& !destTile->Collision())
	//{
	//	glm::vec3 newPosition;
	//	bool hasReachedDestination = glm::length(simplePath) < 0.1f;
	//	if (hasReachedDestination)
	//	{
	//		newPosition = unit->GetPosition() + simplePath;
	//	}
	//	else
	//	{
	//		newPosition = unit->GetPosition() + (glm::normalize(simplePath) / 10.0f);
	//	}

	//	unit->SetPosition(newPosition);
	//	return hasReachedDestination;
	//}
	//else
	//{
	//	return true;
	//}

	return false;
}

void MoveAction::draw()
{
	GLObject::GLAsset("pixelflag.png")->Render(destination);
}

// IDLE DEFEND ACTION ------------------------------------------------


IdleDefendAction::IdleDefendAction(float range)
{
	this->range = range;
}

IdleDefendAction::~IdleDefendAction()
{

}

bool IdleDefendAction::execute(clock_t& tick, Unit* unit)
{
	//float dist = 10000.0f;
	//std::vector<Unit*> units;

	//if (unit->friendly)
	//{
	//	units = level->getEnemyUnits();
	//}
	//else
	//{
	//	units = level->getFriendlyUnits();
	//}

	//Unit* target = nullptr;
	//for (Unit* targetUnit : units)
	//{
	//	if (targetStructure && entity->OBJECT_TYPE == ObjectType::STRUCTURE)
	//	{
	//		float distToEntity = glm::length(entity->getPosition() - getPosition());
	//		if (distToEntity < dist)
	//		{
	//			dist = distToEntity;
	//			target = entity;
	//		}
	//	}

	//	if ((targetUnit)->isDead) continue;
	//	float distToEntity = glm::length(targetUnit->GetPosition() - unit->GetPosition());
	//	if (distToEntity < range)
	//	{
	//		dist = distToEntity;
	//		target = targetUnit;

	//		unit->addAction(new TargetAction(target), false);
	//		break;
	//	}
	//}

	return false;
}

void IdleDefendAction::draw()
{

}

// IDLE ATTACK ACTION ------------------------------------------------


IdleAttackAction::IdleAttackAction()
{

}

IdleAttackAction::~IdleAttackAction()
{

}

bool IdleAttackAction::execute(clock_t& tick, Unit* unit)
{
	//std::vector<Unit*> units;

	//if (unit->friendly)
	//{
	//	units = level->getEnemyUnits();
	//}
	//else
	//{
	//	units = level->getFriendlyUnits();
	//}

	//float dist = 10000.0f;
	//Unit* target = nullptr;
	//for (Unit* targetUnit : units)
	//{
	//	if ((targetUnit)->isDead) continue;
	//	float distToEntity = glm::length(targetUnit->GetPosition() - unit->GetPosition());
	//	if (distToEntity < dist)
	//	{
	//		dist = distToEntity;
	//		target = targetUnit;
	//	}
	//}

	//if (target)
	//{
	//	unit->addAction(new TargetAction(target), false);
	//}

	return false;
}

void IdleAttackAction::draw()
{

}

// BUILD OR REPAIR ACTION ------------------------------------------------


BuildOrRepairAction::BuildOrRepairAction(Structure* structure)
{
	type = ActionType::BuildOrRepair;
	target = structure;
}

BuildOrRepairAction::~BuildOrRepairAction()
{

}

bool BuildOrRepairAction::execute(clock_t& tick, Unit* unit)
{
	//glm::vec3 simplePath = target->GetPosition() - unit->GetPosition();
	//Tile* sourceTile = level->GetTileFromCoords(unit->GetPosition());
	//Tile* destTile = level->GetTileFromCoords(target->GetPosition());
	//if (sourceTile
	//	&& destTile
	//	&& !destTile->Collision())
	//{
	//	glm::vec3 newPosition;
	//	if (glm::length(simplePath) > 1.0f)
	//	{
	//		newPosition = unit->GetPosition() + (glm::normalize(simplePath) / 10.0f);
	//		unit->SetPosition(newPosition);
	//	}
	//	//if (glm::length(simplePath) < 1.5f)
	//	//{
	//	//	target->build(0.5f);
	//	//	return target->built;
	//	//}

	//}

	return false;
}

void BuildOrRepairAction::draw()
{
	glm::vec3 pos = target->GetPosition();
	GLObject::GLAsset("greentarget.png")->Render(pos);
}