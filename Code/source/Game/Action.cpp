#include <Game/Action.h>
#include <Game/Level.h>

#include <Utility/Config.h>

#include <random>

REGISTER_ACTION("ChaseAttack", IdleChaseAttackAction);
REGISTER_ACTION("WanderAttack", IdleWanderAttackAction);

const float tileSize = GetConfig("Shared").tileSize;

std::random_device rd;
std::mt19937 rng(rd());

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

// IDLE ACTION ------------------------------------------------


IdleAction::IdleAction(float durationMS)
	: m_durationMS(durationMS)
	, m_startTime(0)
{

}

IdleAction::~IdleAction()
{

}

bool IdleAction::Execute(clock_t& tick, Unit* unit)
{
	if (m_startTime == 0.0f)
	{
		m_startTime = tick;
		unit->SetState(Unit::State::IDLE);
	}

	float msSpentInCurrentAction = ((tick - m_startTime) * 1000 / (float)CLOCKS_PER_SEC);
	return !(msSpentInCurrentAction < m_durationMS);
}


// MOVE ACTION ------------------------------------------------


MoveAction::MoveAction(const glm::vec3& destination)
	: m_destination(destination)
{

}

MoveAction::~MoveAction()
{

}

bool MoveAction::Execute(clock_t& tick, Unit* unit)
{
	unit->StartDodge(m_destination, tick);
	unit->MoveToNextPosition(tick);
	return unit->GetPosition() == m_destination;
}



// IDLE CHASE ATTACK ACTION ------------------------------------------------


IdleChaseAttackAction::IdleChaseAttackAction()
{

}

IdleChaseAttackAction::~IdleChaseAttackAction()
{

}

bool IdleChaseAttackAction::Execute(clock_t& tick, Unit* unit)
{
	// move to player if not currently attacking
	if (unit->GetState() != Unit::State::ATTACKING)
	{
		Unit* player = Level::Get()->GetClosestPlayerUnit(unit->GetPosition());
		glm::vec3 dirToPlayer = player->GetPosition() - unit->GetPosition();
		dirToPlayer.z = 0.0f;
		float dist = glm::length(dirToPlayer);
		if (dist > 20.0f)
		{
			unit->SetState(Unit::State::IDLE);
		}
		// if within view, walk to player
		else if (dist < 10.0f && dist > 0.5f)
		{
			unit->SetState(Unit::State::MOVING);
			unit->SetDirection(dirToPlayer);
			unit->MoveToNextPosition(tick);
		}
		// if in attack range, attack
		else if (dist < 0.5f)
		{
			unit->BasicAttack(unit->GetDirection());
		}
	}

	return false;
}


// IDLE WANDER ATTACK ACTION ------------------------------------------------


IdleWanderAttackAction::IdleWanderAttackAction()
{

}

IdleWanderAttackAction::~IdleWanderAttackAction()
{

}

bool IdleWanderAttackAction::Execute(clock_t& tick, Unit* unit)
{
	if (unit->GetState() == Unit::State::ATTACKING)
	{
		return false;
	}

	if (m_queuedActions.empty())
	{
		std::uniform_int_distribution<std::mt19937::result_type> shouldMoveToTile(0, 1);

		if (shouldMoveToTile(rng))
		{
			// get all traversable tiles and randomly move to one
			std::vector<Tile*> tilesAffected = Level::Get()->GetTilesFromCoords(unit->GetPosition(), tileSize, unit->GetDirection());

			std::vector<Tile*> destCandidates;

			for (auto tile : tilesAffected)
			{
				Tile::TraversalType traversalType = unit->GetMetadata().friendly ? Tile::TraversalType::Friendly : Tile::TraversalType::Enemy;
				float distToTile = glm::length(tile->GetPosition() - unit->GetPosition());
				if (tile->Traversable(traversalType) && distToTile <= tileSize + 0.1f && distToTile > 0.1f) // TODO: magic numbers are bad
				{
					destCandidates.push_back(tile);
				}
			}

			if (destCandidates.empty())
			{
				m_queuedActions.push_back(std::make_shared<IdleAction>(2000.0f));
			}
			else
			{
				std::uniform_int_distribution<std::mt19937::result_type> randomIndexGenerator(0, (int)destCandidates.size() - 1);
				int randomIndex = randomIndexGenerator(rng);

				m_queuedActions.push_back(std::make_shared<MoveAction>(destCandidates[randomIndex]->GetPosition()));
			}
		}
		else
		{
			m_queuedActions.push_back(std::make_shared<IdleAction>(2000.0f));
		}

	}

	if (m_queuedActions.front()->Execute(tick, unit))
	{
		m_queuedActions.pop_front();
	}

	if (unit->GetState() == Unit::State::IDLE)
	{
		Unit* player = Level::Get()->GetClosestPlayerUnit(unit->GetPosition());
		glm::vec3 dirToPlayer = player->GetPosition() - unit->GetPosition();
		if (dirToPlayer.y == 0.0f)
		{
			unit->BasicAttack(glm::vec3(-1.0f, 0.0f, 0.0f));
		}
	}

	return false;
}

