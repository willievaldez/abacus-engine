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

	Unit* priest = new Unit("datASSet.png");
	addUnit(priest);
	priest->setPosition(spawn);
	unitGroups[0].push_back(priest);
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
		printf("Could not open file %s\n", filepath.c_str());
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


	// parse real position of enemy spawner location
	for (std::pair<int, int> spawnerLocation : spawnerLocations)
	{
		glm::vec3 spawnPos;
		spawnPos.x = (spawnerLocation.first * TILE_SIZE) + (TILE_SIZE / 2.0f);
		spawnPos.y = ((tileGrid.size() - spawnerLocation.second) * TILE_SIZE) + (TILE_SIZE / 2.0f);
		spawnPos.z = 0.0f;

		placeStructure(spawnPos, GLObject::Asset("pentagram.png"));

	}

	printf("Grid len: %d, Grid Width: %d\n", tileGrid.size(), tileGrid[0].size());
}

void Level::update(clock_t tick)
{
	for (Structure* structure : structures)
	{
		if (structure->built)
		{
			if (structure->STRUCTURE_TYPE == StructureType::TURRET)
			{
				((Turret*)structure)->damageEnemyWithinRange(enemyUnits);
			}
			else if (structure->STRUCTURE_TYPE == StructureType::SPAWNER)
			{
				Unit* unit = ((Spawner*)structure)->spawn(tick);
				if (unit)
				{
					if (!unit->friendly)
					{
						unit->targetNearestEntity(entities);
					}
					addUnit(unit);
				}
			}
		}
	}

	moveUnits(friendlyUnits);
	moveUnits(enemyUnits);
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

	for (GLObject* entity : entities)
	{
		delete entity;
	}
}

void Level::render()
{

	for (std::vector<Tile*> tileRow : tileGrid)
	{
		for (Tile* tile : tileRow)
		{
			tile->render();
		}
	}

	for (int i = 0; i < entities.size(); i++)
	{
		entities[i]->render();
	}

	if (!unitGroups[0].empty())
	{
		if (unitGroups[0][0]->target)
		{
			bool green = true;
			if (unitGroups[0][0]->target->OBJECT_TYPE == ObjectType::UNIT)
			{
				green = ((Unit*)(unitGroups[0][0]->target))->friendly;
			}
			else if (unitGroups[0][0]->target->OBJECT_TYPE == ObjectType::STRUCTURE)
			{
				green = true;
			}
			unitGroups[0][0]->target->drawSelectedMarker(green);
		}

		for (Unit* unit : unitGroups[0])
		{
			unit->drawSelectedMarker(true);
		}
	}

}

glm::vec3 Level::getSpawn()
{
	return spawn;
}

int Level::addUnit(Unit* unit)
{
	entities.push_back(unit);
	if (unit->friendly) friendlyUnits.push_back(unit);
	else enemyUnits.push_back(unit);
	return entities.size() - 1;
}

void Level::placeStructure(glm::vec3& coords, GLint type)
{
	std::pair<int, int> tileCoords;
	if (getTileFromCoords(coords, tileCoords) && tileGrid[tileCoords.first][tileCoords.second]->traversable)
	{
		glm::vec3 tilePos = tileGrid[tileCoords.first][tileCoords.second]->getPosition();
		Structure* structure = nullptr;
		if (type == GLObject::Asset("turret.png")) structure = new Turret(tilePos);
		else if (type == GLObject::Asset("AstroChurch.png")) structure = new Spawner(tilePos, true);
		else if (type == GLObject::Asset("pentagram.png")) structure = new Spawner(tilePos, false);

		if (structure)
		{
			tileGrid[tileCoords.first][tileCoords.second]->addStructure(structure);
			entities.push_back(structure);
			structures.push_back(structure);

			for (Unit* unit : unitGroups[0])
			{
				unit->target = structure;
			}
		}

	}
}

void Level::moveUnitToTarget(Unit* unit)
{

	glm::vec3 simplePath = unit->target->getPosition() - unit->getPosition();
	std::pair<int, int> sourceTile, destTile;
	if (getTileFromCoords(unit->getPosition(), sourceTile)
		&& getTileFromCoords(unit->target->getPosition(), destTile)
		&& tileGrid[destTile.first][destTile.second]->traversable)
	{
		glm::vec3 newPosition;
		if (glm::length(simplePath) > 1.0f)
		{
			newPosition = unit->getPosition() + (glm::normalize(simplePath) / 10.0f);
			unit->setPosition(newPosition);
		}
		else if (glm::length(simplePath) < 1.5f)
		{
			if (unit->target->OBJECT_TYPE == ObjectType::UNIT)
			{
				((Unit*)(unit->target))->takeDamage(0.2f);
				if (((Unit*)(unit->target))->isDead)
				{
					unit->target = nullptr;
					if (!unit->friendly)
					{
						unit->targetNearestEntity(entities);

					}
				}
			}
			else if (unit->target->OBJECT_TYPE == ObjectType::STRUCTURE)
			{
				((Structure*)(unit->target))->build(0.5f);
				if (((Structure*)(unit->target))->built)
				{
					unit->target = nullptr;
				}
			}

		}

	}

}

void Level::moveUnitToDestination(Unit* entity)
{
	glm::vec3 simplePath = entity->destinations.back() - entity->getPosition();
	std::pair<int, int> sourceTile, destTile;
	if (getTileFromCoords(entity->getPosition(), sourceTile) && getTileFromCoords(entity->destinations.back(), destTile) && tileGrid[destTile.first][destTile.second]->traversable)
	{
		glm::vec3 newPosition;
		if (glm::length(simplePath) < 0.1f)
		{
			newPosition = entity->getPosition() + simplePath;
			entity->destinations.pop_back();
		}
		else
		{
			newPosition = entity->getPosition() + (glm::normalize(simplePath) / 10.0f);
		}

		entity->setPosition(newPosition);
	}
	else
	{
		entity->destinations.pop_back();
	}
}

void Level::moveUnits(std::vector<Unit*>& units)
{
	for (int i = 0; i < units.size(); i++)
	{
		while (i < units.size() && units[i]->isDead)
		{
			units.erase(units.begin()+i);
		}

		if (i >= units.size()) break;

		if (units[i]->target)
		{
			moveUnitToTarget(units[i]);
		}
		else if (!units[i]->destinations.empty())
		{
			moveUnitToDestination(units[i]);
		}

	}
}

bool Level::getTileFromCoords(glm::vec3 dest, std::pair<int, int>& tileCoords)
{
	if (dest.x < 0.0f || dest.y < 0.0f
		|| dest.x >(tileGrid[0].size() * TILE_SIZE) || dest.y >(tileGrid.size() * TILE_SIZE))
	{
		return false;
	}

	tileCoords.second = dest.x / TILE_SIZE;
	tileCoords.first = ceil(((tileGrid.size() * TILE_SIZE) - dest.y) / TILE_SIZE);

	return true;
}

bool Level::getCoordsFromTile(std::pair<int, int> tileCoords, glm::vec3& dest)
{
	if (tileCoords.first < 0 || tileCoords.first >= tileGrid.size()
		|| tileCoords.second < 0 || tileCoords.second >= tileGrid[0].size())
	{
		return false;
	}

	dest.x = (tileCoords.second * TILE_SIZE) + (TILE_SIZE / 2.0f);
	dest.y = ((tileGrid.size() - tileCoords.first) * TILE_SIZE) + (TILE_SIZE / 2.0f);
	dest.z = 0.0f;

	return true;
}

GLObject* Level::getEntityFromCoords(glm::vec3& coords)
{
	for (GLObject* entity : entities)
	{
		glm::vec3 distToEntity = entity->getPosition() - coords;
		if (glm::length(distToEntity) < 0.6f)
		{
			return entity;
		}
	}

	return nullptr;
}

void Level::addTarget(glm::vec3& coords, bool modifier)
{
	GLObject* targetedEntity = getEntityFromCoords(coords);

	for (Unit* activeUnit : unitGroups[0])
	{
		if (!modifier) activeUnit->destinations.clear();
		if (targetedEntity
			&& targetedEntity->OBJECT_TYPE == ObjectType::UNIT
			&& !((Unit*)targetedEntity)->isDead
			&& ((Unit*)targetedEntity)->friendly != activeUnit->friendly)
		{
			activeUnit->target = targetedEntity;
		}
		else activeUnit->destinations.push_back(coords);
	}
}

void Level::selectUnit(glm::vec3& coords, bool modifier)
{
	GLObject* targetedEntity = getEntityFromCoords(coords);

	if (targetedEntity)
	{
		if (targetedEntity->OBJECT_TYPE == ObjectType::UNIT)
		{
			Unit* targetedUnit = (Unit*)targetedEntity;

			if (modifier)
			{
				unitGroups[0].push_back(targetedUnit);
			}
			else
			{
				unitGroups[0].clear();
				unitGroups[0].push_back(targetedUnit);
			}
		}
		else if (targetedEntity->OBJECT_TYPE == ObjectType::STRUCTURE)
		{
			// TODO: show range and what not
		}
	}
	else
	{
		unitGroups[0].clear();
	}

}