#include <Game/Level.h>

#include <glm/gtc/matrix_transform.hpp> // translate
#include <fstream>
#include <sstream>

#include <Config.h>
#include <GLWrapper/Window.h>

Level* Level::Get()
{
	static Level* level = nullptr;
	if (!level)
	{
		level = new Level(GetConfig().level.c_str());
		level->m_player = Unit::Create("Lumaton");
		level->m_player->SetPosition(level->m_spawn);
		level->m_attack = new GLObject("attack.png");
		level->m_tickTime = clock();
	}
	return level;
}

Level::Level(const char* lvlFile)
{
	m_filepath = INSTALL_DIR + "Assets/2D/Levels/" + lvlFile;
	MakeLevelFromFile();
}

void Level::Reload()
{
	for (std::vector<Tile*> tileRow : m_tileGrid)
	{
		for (Tile* tile : tileRow)
		{
			delete tile;
		}

		tileRow.clear();
	}

	m_tileGrid.clear();

	MakeLevelFromFile();
}

void Level::MakeLevelFromFile()
{
	std::string line;
	std::ifstream myfile(m_filepath);
	std::pair<int, int> spawnGridLocation;
	spawnGridLocation.first = -1;
	spawnGridLocation.second = -1;
	std::vector<std::pair<int, int>> spawnerLocations;
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			std::vector<Tile*> gridRow;

			std::stringstream lineStream(line);
			std::string val;
			while (getline(lineStream, val, ','))
			{
				if (val == "Spawn")
				{
					spawnGridLocation.first = (int)gridRow.size();
					spawnGridLocation.second = (int)m_tileGrid.size();

					printf("Found spawn location at tile (%d, %d)\n", (int)m_tileGrid.size(), (int)gridRow.size());
				}
				else if (val == "Summoning Circle")
				{
					spawnerLocations.push_back(std::pair<int, int>((int)gridRow.size(), (int)m_tileGrid.size()));
				}

				gridRow.push_back(new Tile(val));
			}
			m_tileGrid.push_back(gridRow);
		}


	}
	else
	{
		printf("Could not open file %s\n", m_filepath.c_str());
		return;
	}

	float tileSize = GetConfig().tileSize;

	// parse real position of player spawn location
	if (spawnGridLocation.first == -1)
	{
		printf("Error: no spawn location provided. please mark a tile with 'Spawn'\n");
	}
	else
	{
		m_spawn.x = (spawnGridLocation.first * tileSize) + (tileSize / 2.0f);
		m_spawn.y = ((m_tileGrid.size() - spawnGridLocation.second) * tileSize) + (tileSize / 2.0f);
		m_spawn.z = 0.0f;
		printf("spwan: (%f, %f)\n", m_spawn.x, m_spawn.y);
	}

	int numRows = (int)m_tileGrid.size();
	glm::vec3 offset(tileSize / 2.0f, tileSize * numRows + (tileSize / 2.0f), 0.0f);

	for (std::vector<Tile*> tileRow : m_tileGrid)
	{
		offset.x = tileSize / 2.0f;
		for (Tile* tile : tileRow)
		{
			tile->SetPosition(offset);
			offset.x += tileSize;
		}
		offset.y -= tileSize;
	}


	// parse real position of enemy spawner location
	for (std::pair<int, int> spawnerLocation : spawnerLocations)
	{
		glm::vec3 spawnPos;
		spawnPos.x = (spawnerLocation.first * tileSize) + (tileSize / 2.0f);
		spawnPos.y = ((m_tileGrid.size() - spawnerLocation.second) * tileSize) + (tileSize / 2.0f);
		spawnPos.z = 1.0f;

		PlaceStructure(spawnPos, GLObject::GLAsset("pentagram.png"));

	}

	printf("Grid len: %d, Grid Width: %d\n", (int)m_tileGrid.size(), (int)m_tileGrid[0].size());
}

void Level::Update(clock_t& tick, GLFWwindow* window)
{
	// get player input and update behavior
	const bool* keyMap = Window::GetKeyMap();
	const Camera& cam = Window::GetCamera();
	glm::vec3 direction(0.0f);
	glm::vec3 cam_direction_no_y(cam.direction.x, 0.0f, cam.direction.z);
	if (keyMap[GLFW_KEY_W] || keyMap[GLFW_KEY_UP]) {
		direction.y += 1.0f;
	}
	if (keyMap[GLFW_KEY_A] || keyMap[GLFW_KEY_LEFT]) {
		direction += glm::cross(cam.up, cam_direction_no_y);
	}
	if (keyMap[GLFW_KEY_S] || keyMap[GLFW_KEY_DOWN]) {
		direction.y -= 1.0f;
	}
	if (keyMap[GLFW_KEY_D] || keyMap[GLFW_KEY_RIGHT]) {
		direction += glm::cross(cam_direction_no_y, cam.up);
	}

	if (!glm::length(direction) == 0.0f)
	{
		direction = glm::normalize(direction);

		glm::vec3 destination(0.0f);
		m_player->GetMovePosition(direction, destination);

		Tile* destTile = GetTileFromCoords(destination);

		if (destTile && !destTile->Collision(destination))
		{
			m_player->setState(State::MOVING);
			m_player->SetPosition(destination);
			Window::SetCameraPos(destination);
		}
	}
	else
	{
		m_player->setState(State::IDLE);
	}

	// basic attack
	if (keyMap[GLFW_MOUSE_BUTTON_1])
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		const Camera& cam = Window::GetCamera();
		glm::vec4 mouseWorldSpaceVec4((xpos - (GetConfig().windowWidth / 2.0)) / (cam.FOV / 2.0f), ((GetConfig().windowHeight / 2.0) - ypos) / (cam.FOV / 2.0f), 0.0f, 1.0f);
		glm::vec3 mouseWorldSpace(mouseWorldSpaceVec4.x + cam.pos.x, mouseWorldSpaceVec4.y + cam.pos.y, 1.9f);
		glm::vec3 attackPos = cam.pos;
		attackPos.z = 1.9f;
		glm::vec3 attackDirection = glm::normalize(mouseWorldSpace - attackPos) / 3.0f;
		attackDirection.z = 0.0f;
		Level::Get()->BasicAttack(attackPos, attackDirection);
	}


	//for (Structure* structure : m_structures)
	//{
	//	if (structure->built)
	//	{
	//		if (structure->STRUCTURE_TYPE == StructureType::TURRET)
	//		{
	//			((Turret*)structure)->damageEnemyWithinRange(m_enemyUnits);
	//		}
	//		else if (structure->STRUCTURE_TYPE == StructureType::SPAWNER)
	//		{
	//			Unit* unit = ((Spawner*)structure)->spawn(tick);
	//			if (unit)
	//			{
	//				printf("spawned unit/n");
	//				addUnit(unit);
	//			}
	//		}
	//	}
	//}

	// update all units (including player)
	for (auto& unit : m_units)
	{
		unit->Update(tick);
	}

	for (std::vector<Tile*> tileRow : m_tileGrid)
	{
		for (Tile* tile : tileRow)
		{
			tile->Update(tick);
		}
	}

	//for (int i = 0; i < m_entities.size(); i++)
	//{
	//	while (i < m_entities.size()
	//		&& m_entities[i]->OBJECT_TYPE == ObjectType::UNIT
	//		&& ((Unit*)m_entities[i])->isDead)
	//	{
	//		//for (int j = 0; j < 5; j++)
	//		//{
	//		//	auto it = std::find(m_unitGroups[j].begin(), m_unitGroups[j].end(), *(m_entities.begin() + i));
	//		//	if (it != m_unitGroups[j].end())
	//		//	{
	//		//		m_unitGroups[j].erase(it);
	//		//	}
	//		//}

	//		//m_deadUnits.push_back((Unit*)*(m_entities.begin() + i));
	//		m_entities.erase(m_entities.begin() + i);
	//		printf("deleting entity, size: %d\n", (int)m_entities.size());
	//	}
	//}

	if (glm::length(m_attackDirection) != 0.0f)
	{
		// check for collision
		float hit = 2.0f;
		Unit* hitUnit = nullptr;
		for (Unit* unit : m_units)
		{
			if (unit != m_player)
			{
				float dist = glm::length(m_attack->GetPosition() - unit->GetPosition());
				if (dist <= hit)
				{
					hit = dist;
					hitUnit = unit;
				}
			}
		}

		if (hitUnit)
		{
			hitUnit->TakeDamage(5.0f);
			m_attackDirection = glm::vec3(0.0f);
		}
		else
		{
			glm::vec3 newAttackPos = m_attack->GetPosition() + m_attackDirection;
			Tile* tile = GetTileFromCoords(newAttackPos);
			if (tile && !tile->Collision(newAttackPos))
			{
				m_attack->SetPosition(newAttackPos);
			}
			else
			{
				m_attackDirection = glm::vec3(0.0f);
			}
		}
	}

	//updateUnits(m_friendlyUnits, tick);
	//updateUnits(m_enemyUnits, tick);
}

Level::~Level()
{
	for (std::vector<Tile*> tileRow : m_tileGrid)
	{
		for (Tile* tile : tileRow)
		{
			delete tile;
		}
	}

	//for (GLObject* entity : m_entities)
	//{
	//	delete entity;
	//}
}


void Level::Render(/*float skew, float rot*/)
{
	UniformContainer::SetUniform("playerHealth", m_player->GetHealth());

	//for (std::vector<Tile*> tileRow : tileGrid)
	//{
	//	for (Tile* tile : tileRow)
	//	{
	//		tile->renderFloor();
	//	}
	//}

	//for (std::vector<Tile*> tileRow : tileGrid)
	//{
	//	for (Tile* tile : tileRow)
	//	{
	//		tile->renderStructure();
	//	}
	//}

	//GLObject::setIsometricSkew(skew, rot);

	for (std::vector<Tile*> tileRow : m_tileGrid)
	{
		for (Tile* tile : tileRow)
		{
			tile->Render();
		}
	}

	for (Unit* unit : m_units)
	{
		if (unit != m_player)
		{
			unit->Render();
		}
	}

	// render player on top of everything else
	m_player->Render();

	if (glm::length(m_attackDirection) != 0.0f)
	{
		m_attack->Render();
	}

	//for (int i = 0; i < m_deadUnits.size(); i++)
	//{
	//	glm::vec3 pos = m_deadUnits[i]->getPosition();
	//	GLObject::GLAsset("gravestone.png")->render(pos);
	//}

	//for (int i = 0; i < m_enemyUnits.size(); i++)
	//{
	//	m_enemyUnits[i]->draw();
	//}

	//for (int i = 0; i < m_friendlyUnits.size(); i++)
	//{
	//	m_friendlyUnits[i]->Render();
	//}

	//if (!unitGroups[0].empty())
	//{
	//	// draw only the actions of the most recently selected unit
	//	if (unitGroups[0].size() > 0)
	//	{
	//		unitGroups[0].back()->drawActions();
	//	}

	//	for (Unit* unit : unitGroups[0])
	//	{
	//		glm::vec3 pos = unit->getPosition();
	//		GLObject::GLAsset("greentarget.png")->render(pos);
	//	}
	//}

}

glm::vec3 Level::getSpawn()
{
	return m_spawn;
}

const std::vector<std::vector<Tile*>>& Level::getTileGrid()
{
	return m_tileGrid;
}

//const std::vector<Unit*>& Level::getFriendlyUnits()
//{
//	return m_friendlyUnits;
//}
//
//const std::vector<Unit*>& Level::getEnemyUnits()
//{
//	return m_enemyUnits;
//}
//const std::vector<Structure*>& Level::getStructures()
//{
//	return m_structures;
//}


int Level::AddUnit(Unit* unit)
{
	m_units.push_back(unit);
	return (int)m_units.size();
}

// TODO: computationally expensive
bool Level::RemoveUnit(Unit* unit)
{
	for (int i = 0; i < m_units.size(); i++)
	{
		if (m_units[i] == unit)
		{
			m_units.erase(m_units.begin() + i);
			return true;
		}
	}

	return false;
}

void Level::PlaceStructure(glm::vec3& coords, Asset* type)
{
	Tile* tile = GetTileFromCoords(coords);
	if (tile && !tile->Collision(coords))
	{
		glm::vec3 tilePos = tile->GetPosition();
		Structure* structure = nullptr;
		if (type == GLObject::GLAsset("turret.png")) structure = new Turret(tilePos);
		else if (type == GLObject::GLAsset("AstroChurch.png") || type == GLObject::GLAsset("pentagram.png")) structure = new Spawner(tilePos, type);

		if (structure)
		{
			tile->AddStructure(structure);
			//entities.push_back(structure);
			//structures.push_back(structure);

			//BuildOrRepairAction* buildAction = new BuildOrRepairAction(structure);

			//for (Unit* unit : unitGroups[0])
			//{
				//unit->target = structure;
			//	unit->addAction(buildAction, true);
			//}
		}

	}
}

void Level::BasicAttack(const glm::vec3& origin, const glm::vec3& direction)
{
	if (glm::length(m_attackDirection) == 0.0f)
	{
		m_attack->SetPosition(origin);
		m_attackDirection = direction;
		m_player->TakeDamage(1.0f);
	}
}

//void Level::updateUnits(std::vector<Unit*>& units, clock_t& tick)
//{
//	for (int i = 0; i < units.size(); i++)
//	{
//		while (i < units.size() && units[i]->isDead)
//		{
//			if (units[i]->friendly)
//			{
//				printf("deleting friendly unit, ");
//			}
//			else
//			{
//				printf("deleting enemy unit, ");
//			}
//			deadUnits.push_back(*(units.begin() + i));
//			units.erase(units.begin()+i);
//			printf("size: %d\n", (int)units.size());
//		}
//
//		if (i >= units.size()) break;
//
//		units[i]->update(tick);
//
//	}
//}

Tile* Level::GetTileFromCoords(glm::vec3 dest)
{
	std::pair<int, int> tileCoords;
	float tileSize = GetConfig().tileSize;
	tileCoords.second = (int)(dest.x / tileSize);
	tileCoords.first = (int)(ceil(((m_tileGrid.size() * tileSize) - dest.y) / tileSize));

	if (tileCoords.first < 0 || tileCoords.first >= m_tileGrid.size()
		|| tileCoords.second < 0 || tileCoords.second >= m_tileGrid[tileCoords.first].size())
	{
		return nullptr;
	}

	return m_tileGrid[tileCoords.first][tileCoords.second];
}

bool Level::getCoordsFromTile(std::pair<int, int> tileCoords, glm::vec3& dest)
{
	if (tileCoords.first < 0 || tileCoords.first >= m_tileGrid.size()
		|| tileCoords.second < 0 || tileCoords.second >= m_tileGrid[0].size())
	{
		return false;
	}

	float tileSize = GetConfig().tileSize;
	dest.x = (tileCoords.second * tileSize) + (tileSize / 2.0f);
	dest.y = ((m_tileGrid.size() - tileCoords.first) * tileSize) + (tileSize / 2.0f);
	dest.z = 0.0f;

	return true;
}

std::vector<GLObject*> Level::getEntitiesFromCoords(glm::vec3& coords)
{
	std::vector<GLObject*> targetedEntities;
	//for (GLObject* entity : m_entities)
	//{
	//	glm::vec3 distToEntity = entity->GetPosition() - coords;
	//	if (glm::length(distToEntity) < 0.6f)
	//	{
	//		targetedEntities.push_back(entity);
	//	}
	//}

	return targetedEntities;
}

//void Level::addTarget(glm::vec3& coords, bool modifier)
//{
//	std::vector<GLObject*> targetedEntities = getEntitiesFromCoords(coords);
//	GLObject* target = nullptr;
//	for (GLObject* targetedEntity : targetedEntities)
//	{
//		if ((targetedEntity->OBJECT_TYPE == ObjectType::UNIT
//			&& !((Unit*)targetedEntity)->isDead
//			&& !((Unit*)targetedEntity)->friendly)
//			|| (targetedEntity->OBJECT_TYPE == ObjectType::STRUCTURE
//				&& !((Structure*)targetedEntity)->built))
//		{
//			target = targetedEntity;
//			break;
//		}
//	}
//
//	for (Unit* activeUnit : unitGroups[0])
//	{
//		if (target)
//		{
//			Action* targetAction = nullptr;
//			if (target->OBJECT_TYPE == ObjectType::UNIT)
//			{
//				targetAction = new TargetAction((Unit*)target);
//			}
//			else if (target->OBJECT_TYPE == ObjectType::STRUCTURE)
//			{
//				targetAction = new BuildOrRepairAction((Structure*)target);
//			}
//			activeUnit->addAction(targetAction, !modifier);
//		}
//		else
//		{
//			MoveAction* moveAction = new MoveAction(coords);
//			activeUnit->addAction(moveAction, !modifier);
//		}
//	}
//}

//void Level::selectUnit(glm::vec3& coords, bool modifier)
//{
//	std::vector<GLObject*> targetedEntities = getEntitiesFromCoords(coords);
//	Unit* targetedUnit = nullptr;
//	for (GLObject* targetedEntity : targetedEntities)
//	{
//		if (targetedEntity
//			&& targetedEntity->OBJECT_TYPE == ObjectType::UNIT
//			&& !((Unit*)targetedEntity)->isDead
//			&& ((Unit*)targetedEntity)->friendly
//			&& std::find(unitGroups[0].begin(), unitGroups[0].end(), targetedEntity) == unitGroups[0].end())
//		{
//			targetedUnit = (Unit*)targetedEntity;
//		}
//	}
//
//	if (!modifier)
//		unitGroups[0].clear();
//
//	if (targetedUnit)
//	{
//		/*if (targetedEntity->OBJECT_TYPE == ObjectType::UNIT)
//		{*/
//			if (targetedUnit->friendly && !targetedUnit->isDead)
//			{
//				printf("adding unit to selection, size: %d\n", (int)unitGroups[0].size());
//				unitGroups[0].push_back(targetedUnit);
//			}
//
//			
//		//}
//		//else if (targetedEntity->OBJECT_TYPE == ObjectType::STRUCTURE)
//		//{
//		//	// TODO: show range and what not
//		//}
//	}
//
//
//}