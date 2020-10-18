#include <Game/Level.h>
#include <Game/Player.h>

#include <fstream>
#include <sstream>

#include <Utility/Config.h>
#include <GLWrapper/Window.h>
#include <GLWrapper/PointLight.h>

// TODO TEMP:
float attenuationUniform = 50.0f;
int attenuationType = 0;

Level* Level::Get()
{
	static Level* level = nullptr;
	if (!level)
	{
		int testVal = 0;
		level = new Level(GetConfig().level.c_str());
		level->m_player = new Player(level->m_spawn);
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

	if (spawnGridLocation.first == -1)
	{
		printf("Error: no spawn location provided. please mark a tile with 'Spawn'\n");
	}
	else
	{
		// now that the m_tileGrid size is known, parse real position of player spawn location
		m_spawn.x = (spawnGridLocation.first * tileSize) + (tileSize / 2.0f);
		m_spawn.y = ((m_tileGrid.size() - spawnGridLocation.second) * tileSize) + (tileSize / 2.0f);
		m_spawn.z = 0.0f;
		Window::SetCameraPos(m_spawn);
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
	for (auto& spawnerLocation : spawnerLocations)
	{
		glm::vec3 spawnPos;
		spawnPos.x = (spawnerLocation.first * tileSize) + (tileSize / 2.0f);
		spawnPos.y = ((m_tileGrid.size() - spawnerLocation.second) * tileSize) + (tileSize / 2.0f);

		Tile* tile = GetTileFromCoords(spawnPos);
		if (tile && !tile->Collision(spawnPos))
		{
			glm::vec3 tilePos = tile->GetPosition();
			Structure* structure = new Spawner(tilePos, GLObject::GLAsset("pentagram.png"));

			if (structure)
			{
				tile->AddStructure(structure);
			}

		}
	}
}

void Level::Update(const clock_t& tick, GLFWwindow* window)
{
	std::vector<Tile*> prevTiles;
	std::vector<Tile*> currTiles;
	Unit* playerUnit = GetPlayerUnit();

	m_player->Update(tick);

	// update all units (including player)
	for (auto& unit : m_units)
	{
		if (unit != playerUnit)
		{
			prevTiles = GetTilesFromCoords(unit->GetPosition(), unit->GetMetadata().hitbox_radius);
			unit->Update(tick);
			currTiles = GetTilesFromCoords(unit->GetPosition(), unit->GetMetadata().hitbox_radius);

			// remove from old tiles, add to new tiles TODO inefficient
			auto prevTileIterator = prevTiles.begin();
			while (prevTileIterator != prevTiles.end())
			{
				(*prevTileIterator)->RemoveUnit(unit);
				prevTileIterator++;
			}

			auto currTileIterator = currTiles.begin();
			while (currTileIterator != currTiles.end())
			{
				(*currTileIterator)->AddUnit(unit);
				currTileIterator++;
			}
		}
	}

	// get player input and update behavior
	const bool* keyMap = Window::GetKeyMap();
	const Camera& cam = Window::GetCamera();

	prevTiles = GetTilesFromCoords(playerUnit->GetPosition(), playerUnit->GetMetadata().hitbox_radius);
	if (keyMap[GLFW_KEY_SPACE] && (playerUnit->GetState() == State::IDLE || playerUnit->GetState() == State::MOVING)) // dodge
	{
		playerUnit->StartDodge();
	}

	glm::vec3 direction(0.0f);
	if (playerUnit->GetState() == State::IDLE || playerUnit->GetState() == State::MOVING) // move
	{
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

		// TODO: TEMP
		if (keyMap[GLFW_KEY_MINUS]) {
			attenuationUniform--;
		}
		else if (keyMap[GLFW_KEY_EQUAL]) {
			attenuationUniform++;
		}
		if (keyMap[GLFW_KEY_1])
		{
			attenuationType = 0;
		}
		else if (keyMap[GLFW_KEY_2])
		{
			attenuationType = 1;
		}
		else if (keyMap[GLFW_KEY_3])
		{
			attenuationType = 2;
		}

		if (glm::length(direction) != 0.0f)
		{
			direction = glm::normalize(direction);
			playerUnit->SetState(State::MOVING);
		}
		else
		{
			playerUnit->SetState(State::IDLE);
		}
	}

	if (playerUnit->GetState() == State::DODGING || playerUnit->GetState() == State::MOVING)
	{
		glm::vec3 destination(0.0f);
		playerUnit->GetMovePosition(direction, destination);

		Tile* destTile = GetTileFromCoords(destination);

		if (destTile && !destTile->Collision(destination))
		{
			playerUnit->SetPosition(destination);
			destTile->Interact(playerUnit);
		}
		else
		{
			// knock back
			//if (hitUnit)
			//{
			//	m_player->TakeDamage(15.0f);
			//	glm::vec3 newPlayerPosition = m_player->GetPosition() + (m_player->GetDirection() * -1.0f);
			//	m_player->SetPosition(newPlayerPosition);
			//}

			playerUnit->SetState(State::IDLE);
		}
	}

	currTiles = GetTilesFromCoords(playerUnit->GetPosition(), playerUnit->GetMetadata().hitbox_radius);

	// remove from old tiles, add to new tiles TODO inefficient
	auto prevTileIterator = prevTiles.begin();
	while (prevTileIterator != prevTiles.end())
	{
		(*prevTileIterator)->RemoveUnit(playerUnit);
		prevTileIterator++;
	}

	auto currTileIterator = currTiles.begin();
	while (currTileIterator != currTiles.end())
	{
		(*currTileIterator)->AddUnit(playerUnit);
		currTileIterator++;
	}

	Window::SetCameraPos(playerUnit->GetPosition()); // always update cam to player position

	// basic attack
	if (keyMap[GLFW_MOUSE_BUTTON_1] && (playerUnit->GetState() == State::IDLE || playerUnit->GetState() == State::MOVING))
	{
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		const Camera& cam = Window::GetCamera();
		glm::vec4 mouseWorldSpaceVec4((xpos - (GetConfig().windowWidth / 2.0)) / (cam.FOV / 2.0f), ((GetConfig().windowHeight / 2.0) - ypos) / (cam.FOV / 2.0f), 0.0f, 1.0f);
		glm::vec3 mouseWorldSpace(mouseWorldSpaceVec4.x + cam.pos.x, mouseWorldSpaceVec4.y + cam.pos.y, 1.9f);
		glm::vec3 attackDirection = glm::normalize(mouseWorldSpace - cam.pos);
		attackDirection.z = 0.0f;
		BasicAttack(attackDirection);
	}

	for (std::vector<Tile*> tileRow : m_tileGrid)
	{
		for (Tile* tile : tileRow)
		{
			tile->Update(tick);
		}
	}
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

}


void Level::Render()
{
	int numLights = (int)m_lightSources.size();
	UniformContainer::SetUniform("numLights", numLights);
	UniformContainer::SetUniform("logFactor", attenuationUniform);
	UniformContainer::SetUniform("attenuationType", attenuationType);
	for (int i = 0; i < numLights; i++)
	{
		std::ostringstream sstream;
		sstream << "lights[" << i << "]";
		UniformContainer::SetUniform(sstream.str(), *m_lightSources[i]);
	}

	std::vector<GLObject*> deferredAssets;
	for (std::vector<Tile*> tileRow : m_tileGrid)
	{
		for (Tile* tile : tileRow)
		{
			tile->Render(deferredAssets);
		}
	}

	for (const auto& asset : deferredAssets)
	{
		asset->Render();
	}

	Unit* playerUnit = GetPlayerUnit();
	for (Unit* unit : m_units)
	{
		if (unit != playerUnit)
		{
			unit->Render();
		}
	}

	// render player on top of everything else
	playerUnit->Render();
}

Unit* Level::GetPlayerUnit()
{
	return m_player->GetUnit();
}


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
			Tile* tile = GetTileFromCoords(unit->GetPosition());
			if (tile)
			{
				tile->RemoveUnit(unit);
				m_units.erase(m_units.begin() + i);
				return true;
			}
		}
	}

	return false;
}

int Level::AddLight(PointLight* light)
{
	m_lightSources.push_back(light);
	return (int)m_lightSources.size();
}

// TODO: computationally expensive
bool Level::RemoveLight(PointLight* light)
{
	for (int i = 0; i < m_lightSources.size(); i++)
	{
		if (m_lightSources[i] == light)
		{
			m_lightSources.erase(m_lightSources.begin() + i);
		}
	}

	return false;
}

void Level::BasicAttack(const glm::vec3& direction)
{
	GetPlayerUnit()->BasicAttack(direction);
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

Tile* Level::GetTileFromCoords(const glm::vec3& dest)
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

std::vector<Tile*> Level::GetTilesFromCoords(const glm::vec3& dest, float radius)
{
	std::vector<Tile*> returnedTiles;

	std::pair<int, int> minTileCoords;
	float tileSize = GetConfig().tileSize;
	minTileCoords.second = (int)((dest.x - radius) / tileSize);
	minTileCoords.first = (int)(ceil(((m_tileGrid.size() * tileSize) - (dest.y - radius)) / tileSize));

	std::pair<int, int> maxTileCoords;
	maxTileCoords.second = (int)((dest.x + radius) / tileSize);
	maxTileCoords.first = (int)(ceil(((m_tileGrid.size() * tileSize) - (dest.y + radius)) / tileSize));

	for (int x = maxTileCoords.first; x <= minTileCoords.first; x++) // iterate in reverse because the mappings are reversed
	{
		if (x >= 0 && x < m_tileGrid.size())
		{
			for (int y = minTileCoords.second; y <= maxTileCoords.second; y++)
			{
				if (y >= 0 && y < m_tileGrid[x].size())
				{
					returnedTiles.push_back(m_tileGrid[x][y]);
				}
			}
		}
	}

	return returnedTiles;
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