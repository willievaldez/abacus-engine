#include <Game/Level.h>

#include <Game/Player.h>
#include <Game/Spawner.h>
#include <Game/Exit.h>

#include <fstream>
#include <sstream>

#include <GLWrapper/Window.h>
#include <GLWrapper/PointLight.h>

#include <Utility/Config.h>

// TODO TEMP:
float attenuationUniform = 50.0f;
int attenuationType = 2;
bool debugHighlight = false;
const Config& config = GetConfig("Shared");

Level* Level::Get()
{
	static Level* level = nullptr;
	if (!level)
	{
		int testVal = 0;
		level = new Level(config.level.c_str());
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

void Level::SetLevelState(LevelState state)
{
	m_levelState = state;
}

void Level::Reload()
{
	SetLevelState(LevelState::INTRO);

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
	std::pair<int, int> playerSpawn(-1,-1);
	using StructureType = std::pair<std::string, std::pair<int, int>>;
	std::vector<StructureType> structureLocations;
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			std::vector<Tile*> gridRow;

			std::stringstream lineStream(line);
			std::string val;
			while (getline(lineStream, val, ','))
			{
				if (val == "Enter")
				{
					playerSpawn.first = (int)gridRow.size();
					playerSpawn.second = (int)m_tileGrid.size();
				}
				else if (val == "Wretch" || val == "Exit")
				{
					structureLocations.push_back({ val, { (int)gridRow.size(), (int)m_tileGrid.size() } });
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

	float tileSize = config.tileSize;

	if (playerSpawn.first == -1)
	{
		printf("Error: no spawn location provided. please mark a tile with 'Spawn'\n");
	}
	else
	{
		// now that the m_tileGrid size is known, parse real position of player spawn location
		m_spawn.x = (playerSpawn.first * tileSize) + (tileSize / 2.0f);
		m_spawn.y = ((m_tileGrid.size() - playerSpawn.second) * tileSize) + (tileSize / 2.0f);
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
	for (auto& structureLocation : structureLocations)
	{
		glm::vec3 structurePos;
		structurePos.x = (structureLocation.second.first * tileSize) + (tileSize / 2.0f);
		structurePos.y = ((m_tileGrid.size() - structureLocation.second.second) * tileSize) + (tileSize / 2.0f);

		Tile* tile = GetTileFromCoords(structurePos);
		if (tile && !tile->Collision(structurePos))
		{
			Structure* structure = nullptr;
			if (structureLocation.first == "Wretch")
			{
				structure = new Spawner(tile->GetPosition(), GLObject::GLAsset("pentagram.png"));
			}
			else if (structureLocation.first == "Exit")
			{
				structure = new Exit(tile->GetPosition(), GLObject::GLAsset("trapdoor.png"));
			}

			if (structure)
			{
				tile->AddStructure(structure);
			}
		}
	}
}

void Level::Update(const KeyMap& keyMap)
{
	clock_t tick = clock();
	//static clock_t lastTick = clock();
	//printf("seconds since last tick: %f\n", (tick - lastTick) / (float)CLOCKS_PER_SEC);
	//lastTick = tick;

	if (m_levelState == LevelState::WON)
	{
		return;
	}
	else if (m_levelState == LevelState::LOST)
	{
		return;
	}
	else if (m_levelState == LevelState::INTRO)
	{
		m_levelState = LevelState::RUNNING;
	}
	else if (m_levelState == LevelState::RUNNING)
	{
		std::vector<Tile*> prevTiles;
		std::vector<Tile*> currTiles;
		Unit* playerUnit = GetPlayerUnit();

		m_player->Update(tick);

		// update all units
		for (auto& unit : m_units)
		{
			if (unit != playerUnit)
			{
				prevTiles = GetTilesFromCoords(unit->GetPosition(), unit->GetMetadata().hitbox_radius);
				unit->Update(tick);
				currTiles = GetTilesFromCoords(unit->GetPosition(), unit->GetMetadata().hitbox_radius);

				// remove from old tiles, add to new tiles TODO inefficient
				for (auto& prevTile : prevTiles)
				{
					prevTile->RemoveUnit(unit);
				}

				for (auto& currTile : currTiles)
				{
					currTile->AddUnit(unit);
				}
			}
		}

		// get player input and update behavior
		const Camera& cam = Window::GetCamera();

		prevTiles = GetTilesFromCoords(playerUnit->GetPosition(), playerUnit->GetMetadata().hitbox_radius);
		if (keyMap[GLFW_KEY_SPACE] && (playerUnit->GetState() == Unit::State::IDLE || playerUnit->GetState() == Unit::State::MOVING)) // dodge
		{
			playerUnit->StartDodge();
		}

		glm::vec3 direction(0.0f);
		if (playerUnit->GetState() == Unit::State::IDLE || playerUnit->GetState() == Unit::State::MOVING) // move
		{
			glm::vec3 cam_direction_no_y(cam.direction.x, 0.0f, cam.direction.z);
			if (keyMap[GLFW_KEY_W] || keyMap[GLFW_KEY_UP])
			{
				direction.y += 1.0f;
			}
			if (keyMap[GLFW_KEY_A] || keyMap[GLFW_KEY_LEFT])
			{
				direction += glm::cross(cam.up, cam_direction_no_y);
			}
			if (keyMap[GLFW_KEY_S] || keyMap[GLFW_KEY_DOWN])
			{
				direction.y -= 1.0f;
			}
			if (keyMap[GLFW_KEY_D] || keyMap[GLFW_KEY_RIGHT])
			{
				direction += glm::cross(cam_direction_no_y, cam.up);
			}

			// TODO: TEMP
			if (keyMap[GLFW_KEY_MINUS])
			{
				attenuationUniform--;
			}
			else if (keyMap[GLFW_KEY_EQUAL])
			{
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

			if (keyMap[GLFW_KEY_4])
			{
				debugHighlight = !debugHighlight;
			}

			if (glm::length(direction) != 0.0f)
			{
				playerUnit->SetState(Unit::State::MOVING);
				playerUnit->SetDirection(direction);
			}
			else
			{
				playerUnit->SetState(Unit::State::IDLE);
			}
		}

		if (playerUnit->GetState() == Unit::State::DODGING || playerUnit->GetState() == Unit::State::MOVING)
		{
			playerUnit->MoveToNextPosition(tick);
		}

		currTiles = GetTilesFromCoords(playerUnit->GetPosition(), playerUnit->GetMetadata().hitbox_radius);

		// remove player unit from old tiles, add to new tiles TODO inefficient
		for (auto& prevTile : prevTiles)
		{
			prevTile->RemoveUnit(playerUnit);
			prevTile->SetDebugHighlight(glm::vec3(0.0f));
		}

		for (auto& currTile : currTiles)
		{
			currTile->AddUnit(playerUnit);
			if (debugHighlight)
			{
				currTile->SetDebugHighlight(glm::vec3(0.0f, 0.2f, 0.0f));
			}
		}

		Window::SetCameraPos(playerUnit->GetPosition()); // always update cam to player position

		// basic attack
		if (keyMap[GLFW_MOUSE_BUTTON_1] && (playerUnit->GetState() == Unit::State::IDLE || playerUnit->GetState() == Unit::State::MOVING))
		{
			glm::vec3 mouseWorldSpace = Window::GetCursorPosWorldSpace();
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

		if (playerUnit->GetHealth() <= 0.0)
		{
			SetLevelState(LevelState::LOST);
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
	if (m_levelState == LevelState::WON)
	{
		glClearColor(0.0f, 0.5f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return;
	}
	else if (m_levelState == LevelState::LOST)
	{
		glClearColor(0.5f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return;
	}

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

Unit* Level::FindUnit(Unit* unit)
{
	auto foundUnit = m_units.find(unit);
	if (foundUnit != m_units.end())
	{
		return *foundUnit;
	}

	return nullptr;
}

int Level::AddUnit(Unit* unit)
{
	m_units.emplace(unit);
	return (int)m_units.size();
}

bool Level::RemoveUnit(Unit* unit)
{
	auto foundUnit = m_units.find(unit);
	if (foundUnit != m_units.end())
	{
		Tile* tile = GetTileFromCoords(unit->GetPosition());
		if (tile)
		{
			tile->RemoveUnit(unit);
			m_units.erase(foundUnit);
			return true;
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

Tile* Level::GetTileFromCoords(const glm::vec3& dest)
{
	std::pair<int, int> tileCoords;
	float tileSize = config.tileSize;
	tileCoords.second = (int)(dest.x / tileSize);
	tileCoords.first = (int)(ceil(((m_tileGrid.size() * tileSize) - dest.y) / tileSize));

	if (tileCoords.first < 0 || tileCoords.first >= m_tileGrid.size()
		|| tileCoords.second < 0 || tileCoords.second >= m_tileGrid[tileCoords.first].size())
	{
		return nullptr;
	}

	return m_tileGrid[tileCoords.first][tileCoords.second];
}

std::pair<int, int> Level::GetTileIndices(const glm::vec3& pos)
{
	std::pair<int, int> tileIndices;
	float tileSize = config.tileSize;
	tileIndices.second = (int)((pos.x) / tileSize);
	tileIndices.first = (int)(ceil(((m_tileGrid.size() * tileSize) - (pos.y)) / tileSize));
	return tileIndices;
}

std::vector<Tile*> Level::GetTilesFromCoords(const glm::vec3& pos, float radius, const glm::vec3& dir)
{
	std::pair<int, int> tileCoords = GetTileIndices(pos);
	std::pair<int, int> minTileCoords = GetTileIndices(pos - radius);
	std::pair<int, int> maxTileCoords = GetTileIndices(pos + radius);

	if (glm::length(dir))
	{
		if (dir.x > 0)
		{
			minTileCoords.first = tileCoords.first;
		}
		else if (dir.x < 0)
		{
			maxTileCoords.first = tileCoords.first;
		}
		if (dir.y > 0)
		{
			minTileCoords.second = tileCoords.second;
		}
		else if (dir.y < 0)
		{
			maxTileCoords.second = tileCoords.second;
		}
	}

	std::vector<Tile*> returnedTiles;
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

	float tileSize = config.tileSize;
	dest.x = (tileCoords.second * tileSize) + (tileSize / 2.0f);
	dest.y = ((m_tileGrid.size() - tileCoords.first) * tileSize) + (tileSize / 2.0f);
	dest.z = 0.0f;

	return true;
}
