#include "Level.h"

#include <glm/gtc/matrix_transform.hpp> // translate
#include <fstream>
#include <sstream>



#define TILE_SIZE 2.0f

Level::Level(const char* lvlFile)
{
	Tile::setTileSize(TILE_SIZE);
	filepath = INSTALL_DIR + "Models/Levels/" + lvlFile;

	makeLevelFromFile();

	tickTime = clock();
}

void Level::reload()
{
	for (std::vector<Tile*> tileRow : tileGrid)
	{
		tileRow.clear();
	}

	tileGrid.clear();

	makeLevelFromFile();
}

void Level::makeLevelFromFile()
{
	std::string line;
	std::ifstream myfile(filepath);
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
					spawnGridLocation.first = gridRow.size();
					spawnGridLocation.second = tileGrid.size();

					printf("Found spawn location at tile (%d, %d)\n", tileGrid.size(), gridRow.size());
				}
				else if (val == "Summoning Circle")
				{
					spawnerLocations.push_back(std::pair<int, int>(gridRow.size(), tileGrid.size()));
				}

				gridRow.push_back(new Tile(val));
			}
			tileGrid.push_back(gridRow);
		}


	}
	else
	{
		printf("Could not open file %s\n", filepath);
	}

	// parse real position of player spawn location
	if (spawnGridLocation.first == -1)
	{
		printf("Error: no spawn location provided. please mark a tile with 'Spawn'\n");
	}
	else
	{
		spawn.x = (spawnGridLocation.first * TILE_SIZE) + (TILE_SIZE / 2.0f);
		spawn.y = ((tileGrid.size() - spawnGridLocation.second) * TILE_SIZE) + (TILE_SIZE / 2.0f);
		printf("spwan: (%f, %f)\n", spawn.x, spawn.y);
	}

	// parse real position of enemy spawner location
	for (std::pair<int, int> spawnerLocation : spawnerLocations)
	{
		glm::vec3 spawnPos;
		spawnPos.x = (spawnerLocation.first * TILE_SIZE) + (TILE_SIZE / 2.0f);
		spawnPos.y = ((tileGrid.size() - spawnerLocation.second) * TILE_SIZE) + (TILE_SIZE / 2.0f);
		spawnPos.z = 0.0f;
		spawners.push_back(new Spawner(spawnPos));
	}

	int numRows = tileGrid.size();
	glm::vec3 offset(TILE_SIZE / 2.0f, TILE_SIZE * numRows + (TILE_SIZE / 2.0f), 0.0f);

	for (std::vector<Tile*> tileRow : tileGrid)
	{
		offset.x = TILE_SIZE / 2.0f;
		for (Tile* tile : tileRow)
		{
			tile->setPosition(offset);
			offset.x += TILE_SIZE;
		}
		offset.y -= TILE_SIZE;
	}

	printf("Grid len: %d, Grid Width: %d\n", tileGrid.size(), tileGrid[0].size());
}

void Level::update(clock_t tick)
{
	for (Spawner* spawner : spawners)
	{
		Enemy* newEnemy = spawner->spawn(tick);
		if (newEnemy)
		{
			addEntity(newEnemy);
		}
	}

	moveEntities();
}

Level::~Level()
{
	Tile::releaseBuffers();

	for (std::vector<Tile*> tileRow : tileGrid)
	{
		for (Tile* tile : tileRow)
		{
			delete tile;
		}
	}
}

void Level::render(GLuint shaderProgram)
{

	int numRows = tileGrid.size();

	for (std::vector<Tile*> tileRow : tileGrid)
	{
		for (Tile* tile : tileRow)
		{
			tile->render(shaderProgram);
		}
	}

	for (Unit* entity : entities)
	{
		entity->render(shaderProgram);
	}

}

glm::vec3 Level::getSpawn()
{
	return spawn;
}

int Level::addEntity(Unit* entity)
{
	entities.push_back(entity);
	return entities.size() - 1;
}

void Level::buildStructure(GLObject* structure)
{
	std::pair<int, int> tileCoords;
	if (getTileFromCoords(structure->getPosition(), tileCoords) && tileGrid[tileCoords.first][tileCoords.second]->traversable)
	{
		tileGrid[tileCoords.first][tileCoords.second]->setAsset(structure->getTextureID());
	}
}

void Level::moveEntity(int entityId, glm::vec3 offset)
{
	//offset = glm::normalize(offset)/10.0f;
	//rooms[0]->collidesWithBorder(entities[entityId]->getPosition(), offset);
	//entities[entityId]->move(offset);
}

void Level::moveEntityToDestination(Unit* entity)
{
	glm::vec3 simplePath = entity->getDestination() - entity->getPosition();
	std::pair<int, int> sourceTile, destTile;
	if (getTileFromCoords(entity->getPosition(), sourceTile) && getTileFromCoords(entity->getDestination(), destTile) && tileGrid[destTile.first][destTile.second]->traversable)
	{
		glm::vec3 newPosition;
		if (glm::length(simplePath) < 0.1f)
		{
			newPosition = entity->getPosition() + simplePath;
			entity->popDestination();
		}
		else
		{
			newPosition = entity->getPosition() + (glm::normalize(simplePath) / 10.0f);
		}

		entity->setPosition(newPosition);
	}
	else
	{
		entity->popDestination();
	}
}

void Level::moveEntities()
{
	for (Unit* entity : entities)
	{
		if (entity->hasDestination())
		{
			moveEntityToDestination(entity);
		}
	}
}


bool Level::getTileFromCoords(glm::vec3 dest, std::pair<int, int>& tileCoords)
{
	if (dest.x < 0.0f || dest.y < 0.0f)
	{
		return false;
	}
	if (dest.x > (tileGrid[0].size() * TILE_SIZE) || dest.y > (tileGrid.size() * TILE_SIZE))
	{
		return false;
	}

	tileCoords.second = dest.x / TILE_SIZE;
	tileCoords.first = ceil(((tileGrid.size() * TILE_SIZE) - dest.y) / TILE_SIZE);

	return true;
}

bool Level::getCoordsFromTile(std::pair<int, int> tileCoords, glm::vec3& dest)
{
	if (tileCoords.first < 0 || tileCoords.first >= tileGrid.size())
	{
		return false;
	}
	if (tileCoords.second < 0 || tileCoords.second >= tileGrid[0].size())
	{
		return false;
	}

	dest.x = (tileCoords.second * TILE_SIZE) + (TILE_SIZE / 2.0f);
	dest.y = ((tileGrid.size() - tileCoords.first) * TILE_SIZE) + (TILE_SIZE / 2.0f);
	dest.z = 0.0f;

	return true;
}

Unit* Level::selectUnit(glm::vec3& coords)
{
	for (Unit* entity : entities)
	{
		glm::vec3 distToEntity = entity->getPosition() - coords;
		if (glm::length(distToEntity) < 0.6f)
		{
			return entity;
		}
	}

	return nullptr;
}