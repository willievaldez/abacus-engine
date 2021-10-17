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
		else if (dist < 10.0f && dist > 0.5f)
		{
			unit->SetState(State::MOVING);
			glm::vec3 newPosition(0.0f);
			unit->GetMovePosition(glm::normalize(dirToPlayer), newPosition);
			unit->SetPosition(newPosition);
		}
		// if in attack range, attack
		else if (dist < 0.5f)
		{
			unit->BasicAttack(unit->GetDirection());
		}
	}

	return false;
}

void IdleAttackAction::Render()
{

}
