#include <Game/Action.h>
#include <Game/Level.h>

REGISTER_ACTION("Attack", IdleAttackAction);

Action::Action()
{
	lastUpdateInterval = clock();
	nextAction = nullptr;
}

Action::~Action()
{

}

/*static*/
std::shared_ptr<Action> Action::CreateAction(const char* actionName)
{
	std::shared_ptr<Action> returnedType = nullptr;
	auto foundType = AccessActions().find(actionName);
	if (foundType != AccessActions().end())
	{
		returnedType = foundType->second();
	}
	return returnedType;
}

//// TARGET ACTION ------------------------------------------------
//
//TargetAction::TargetAction(Unit* target)
//{
//	this->target = target;
//}
//
//TargetAction::~TargetAction()
//{
//
//}
//
//bool TargetAction::Execute(clock_t& tick, Unit* unit)
//{
//	//glm::vec3 simplePath = target->GetPosition() - unit->GetPosition();
//	//Tile* sourceTile = level->GetTileFromCoords(unit->GetPosition());
//	//Tile* destTile = level->GetTileFromCoords(target->GetPosition());
//	//if (!target->isDead
//	//	&& sourceTile
//	//	&& destTile
//	//	&& !destTile->Collision())
//	//{
//	//	glm::vec3 newPosition;
//	//	if (glm::length(simplePath) > 1.0f)
//	//	{
//	//		newPosition = unit->GetPosition() + (glm::normalize(simplePath) / 10.0f);
//	//		unit->SetPosition(newPosition);
//	//		unit->SetState(State::MOVING);
//	//	}
//	//	if (glm::length(simplePath) < 1.5f)
//	//	{
//	//		target->TakeDamage(0.2f);
//	//		if (!target->isDead)
//	//		{
//	//			unit->SetState(State::ATTACKING);
//	//			return true;
//	//		}
//	//	}
//	//}
//
//	unit->SetState(State::IDLE);
//	return false;
//}
//
//void TargetAction::Render()
//{
//	GLObject::GLAsset("redtarget.png")->Render(target->GetPosition());
//}
//
//// MOVE ACTION ------------------------------------------------
//
//MoveAction::MoveAction(glm::vec3& dest)
//{
//	destination = dest;
//}
//
//MoveAction::~MoveAction()
//{
//
//}
//
//bool MoveAction::Execute(clock_t& tick, Unit* unit)
//{
//	//glm::vec3 simplePath = destination - unit->GetPosition();
//	//Tile* sourceTile = level->GetTileFromCoords(unit->GetPosition());
//	//Tile* destTile = level->GetTileFromCoords(simplePath);
//	//if (sourceTile
//	//	&& destTile
//	//	&& !destTile->Collision())
//	//{
//	//	glm::vec3 newPosition;
//	//	bool hasReachedDestination = glm::length(simplePath) < 0.1f;
//	//	if (hasReachedDestination)
//	//	{
//	//		newPosition = unit->GetPosition() + simplePath;
//	//	}
//	//	else
//	//	{
//	//		newPosition = unit->GetPosition() + (glm::normalize(simplePath) / 10.0f);
//	//	}
//
//	//	unit->SetPosition(newPosition);
//	//	return hasReachedDestination;
//	//}
//	//else
//	//{
//	//	return true;
//	//}
//
//	return false;
//}
//
//void MoveAction::Render()
//{
//	GLObject::GLAsset("pixelflag.png")->Render(destination);
//}
//
//// IDLE DEFEND ACTION ------------------------------------------------
//
//
//IdleDefendAction::IdleDefendAction(float range)
//{
//	this->range = range;
//}
//
//IdleDefendAction::~IdleDefendAction()
//{
//
//}
//
//bool IdleDefendAction::Execute(clock_t& tick, Unit* unit)
//{
//	//float dist = 10000.0f;
//	//std::vector<Unit*> units;
//
//	//if (unit->friendly)
//	//{
//	//	units = level->getEnemyUnits();
//	//}
//	//else
//	//{
//	//	units = level->getFriendlyUnits();
//	//}
//
//	//Unit* target = nullptr;
//	//for (Unit* targetUnit : units)
//	//{
//	//	if (targetStructure && entity->OBJECT_TYPE == ObjectType::STRUCTURE)
//	//	{
//	//		float distToEntity = glm::length(entity->getPosition() - getPosition());
//	//		if (distToEntity < dist)
//	//		{
//	//			dist = distToEntity;
//	//			target = entity;
//	//		}
//	//	}
//
//	//	if ((targetUnit)->isDead) continue;
//	//	float distToEntity = glm::length(targetUnit->GetPosition() - unit->GetPosition());
//	//	if (distToEntity < range)
//	//	{
//	//		dist = distToEntity;
//	//		target = targetUnit;
//
//	//		unit->addAction(new TargetAction(target), false);
//	//		break;
//	//	}
//	//}
//
//	return false;
//}
//
//void IdleDefendAction::Render()
//{
//
//}

// IDLE ATTACK ACTION ------------------------------------------------


IdleAttackAction::IdleAttackAction()
{

}

IdleAttackAction::~IdleAttackAction()
{

}

bool IdleAttackAction::Execute(clock_t& tick, Unit* unit)
{
	// move to player if not currently attacking
	if (unit->GetState() != State::ATTACKING)
	{
		Unit* player = Level::Get()->GetPlayerUnit();
		glm::vec3 dirToPlayer = player->GetPosition() - unit->GetPosition();
		dirToPlayer.z = 0.0f;
		float dist = glm::length(dirToPlayer);
		if (dist > 20.0f)
		{
			unit->SetState(State::IDLE);
		}
		// if within view, walk to player
		else if (dist < 20.0f && dist > 1.0f)
		{
			unit->SetState(State::MOVING);
			glm::vec3 newPosition(0.0f);
			unit->GetMovePosition(glm::normalize(dirToPlayer), newPosition);
			unit->SetPosition(newPosition);
		}
		// if in attack range, attack
		else if (dist < 1.0f)
		{
			unit->BasicAttack(unit->GetDirection());
		}
	}

	return false;
}

void IdleAttackAction::Render()
{

}

// BUILD OR REPAIR ACTION ------------------------------------------------
//
//
//BuildOrRepairAction::BuildOrRepairAction(Structure* structure)
//{
//	target = structure;
//}
//
//BuildOrRepairAction::~BuildOrRepairAction()
//{
//
//}
//
//bool BuildOrRepairAction::Execute(clock_t& tick, Unit* unit)
//{
//	//glm::vec3 simplePath = target->GetPosition() - unit->GetPosition();
//	//Tile* sourceTile = level->GetTileFromCoords(unit->GetPosition());
//	//Tile* destTile = level->GetTileFromCoords(target->GetPosition());
//	//if (sourceTile
//	//	&& destTile
//	//	&& !destTile->Collision())
//	//{
//	//	glm::vec3 newPosition;
//	//	if (glm::length(simplePath) > 1.0f)
//	//	{
//	//		newPosition = unit->GetPosition() + (glm::normalize(simplePath) / 10.0f);
//	//		unit->SetPosition(newPosition);
//	//	}
//	//	//if (glm::length(simplePath) < 1.5f)
//	//	//{
//	//	//	target->build(0.5f);
//	//	//	return target->built;
//	//	//}
//
//	//}
//
//	return false;
//}
//
//void BuildOrRepairAction::Render()
//{
//	glm::vec3 pos = target->GetPosition();
//	GLObject::GLAsset("greentarget.png")->Render(pos);
//}