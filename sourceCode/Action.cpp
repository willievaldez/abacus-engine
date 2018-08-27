#include "Action.h"

Level* Action::level;

Action::Action()
{
	lastUpdateInterval = clock();
	nextAction = nullptr;
}

Action::~Action()
{

}

void Action::setLevel(Level* lvl)
{
	Action::level = lvl;
}


// TARGET ACTION ------------------------------------------------

TargetAction::TargetAction(Unit* target)
{
	this->target = target;
}

TargetAction::~TargetAction()
{

}

bool TargetAction::execute(clock_t& tick, Unit* unit)
{
	glm::vec3 simplePath = target->getPosition() - unit->getPosition();
	std::pair<int, int> sourceTile, destTile;
	if (level->getTileFromCoords(unit->getPosition(), sourceTile)
		&& level->getTileFromCoords(target->getPosition(), destTile)
		&& level->getTileGrid()[destTile.first][destTile.second]->traversable)
	{
		glm::vec3 newPosition;
		if (glm::length(simplePath) > 1.0f)
		{
			newPosition = unit->getPosition() + (glm::normalize(simplePath) / 10.0f);
			unit->setPosition(newPosition);
		}
		if (glm::length(simplePath) < 1.5f)
		{
			target->takeDamage(0.2f);
			return target->isDead;
		}
	}

	return false;
}

void TargetAction::draw()
{
	target->drawSelectedMarker(false);
}

// MOVE ACTION ------------------------------------------------

MoveAction::MoveAction(glm::vec3& dest)
{
	destination = dest;
}

MoveAction::~MoveAction()
{

}

bool MoveAction::execute(clock_t& tick, Unit* unit)
{
	glm::vec3 simplePath = destination - unit->getPosition();
	std::pair<int, int> sourceTile, destTile;
	if (level->getTileFromCoords(unit->getPosition(), sourceTile)
		&& level->getTileFromCoords(destination, destTile)
		&& level->getTileGrid()[destTile.first][destTile.second]->traversable)
	{
		glm::vec3 newPosition;
		bool hasReachedDestination = glm::length(simplePath) < 0.1f;
		if (hasReachedDestination)
		{
			newPosition = unit->getPosition() + simplePath;
		}
		else
		{
			newPosition = unit->getPosition() + (glm::normalize(simplePath) / 10.0f);
		}

		unit->setPosition(newPosition);
		return hasReachedDestination;
	}
	else
	{
		return true;
	}

}

void MoveAction::draw()
{
	GLObject::drawDestinationFlag(destination);
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
	float dist = 10000.0f;
	std::vector<Unit*> units;

	if (unit->friendly)
	{
		units = level->getEnemyUnits();
	}
	else
	{
		units = level->getFriendlyUnits();
	}

	Unit* target = nullptr;
	for (Unit* targetUnit : units)
	{
		//if (targetStructure && entity->OBJECT_TYPE == ObjectType::STRUCTURE)
		//{
		//	float distToEntity = glm::length(entity->getPosition() - getPosition());
		//	if (distToEntity < dist)
		//	{
		//		dist = distToEntity;
		//		target = entity;
		//	}
		//}

		if ((targetUnit)->isDead) continue;
		float distToEntity = glm::length(targetUnit->getPosition() - unit->getPosition());
		if (distToEntity < range)
		{
			dist = distToEntity;
			target = targetUnit;

			unit->addAction(new TargetAction(target), false);
			break;
		}
	}

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
	std::vector<Unit*> units;

	if (unit->friendly)
	{
		units = level->getEnemyUnits();
	}
	else
	{
		units = level->getFriendlyUnits();
	}

	float dist = 10000.0f;
	Unit* target = nullptr;
	for (Unit* targetUnit : units)
	{
		if ((targetUnit)->isDead) continue;
		float distToEntity = glm::length(targetUnit->getPosition() - unit->getPosition());
		if (distToEntity < dist)
		{
			dist = distToEntity;
			target = targetUnit;
		}
	}

	if (target)
	{
		unit->addAction(new TargetAction(target), false);
	}

	return false;
}

void IdleAttackAction::draw()
{

}

// BUILD OR REPAIR ACTION ------------------------------------------------


BuildOrRepairAction::BuildOrRepairAction(Structure* structure)
{
	target = structure;
}

BuildOrRepairAction::~BuildOrRepairAction()
{

}

bool BuildOrRepairAction::execute(clock_t& tick, Unit* unit)
{
	glm::vec3 simplePath = target->getPosition() - unit->getPosition();
	std::pair<int, int> sourceTile, destTile;
	if (level->getTileFromCoords(unit->getPosition(), sourceTile)
		&& level->getTileFromCoords(target->getPosition(), destTile)
		&& level->getTileGrid()[destTile.first][destTile.second]->traversable)
	{
		glm::vec3 newPosition;
		if (glm::length(simplePath) > 1.0f)
		{
			newPosition = unit->getPosition() + (glm::normalize(simplePath) / 10.0f);
			unit->setPosition(newPosition);
		}
		if (glm::length(simplePath) < 1.5f)
		{
			target->build(0.5f);
			return target->built;
		}

	}

	return false;
}

void BuildOrRepairAction::draw()
{
	target->drawSelectedMarker(true);
}