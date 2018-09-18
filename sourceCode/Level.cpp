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
	Action::setLevel(this);

	Unit* priest = new Unit("datASSet.png", true);
	addUnit(priest);
	priest->setPosition(spawn);
	unitGroups[0].push_back(priest);
	tickTime = clock();
}

void Level::reload()
{
	for (std::vector<Tile*> tileRow : tileGrid)
	{
		for (Tile* tile : tileRow)
		{
			delete tile;
		}

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

void Level::update(clock_t& tick)
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
					printf("spawned unit/n");
					addUnit(unit);
				}
			}
		}
	}

	for (int i = 0; i < entities.size(); i++)
	{
		while (i < entities.size() 
			&& entities[i]->OBJECT_TYPE == ObjectType::UNIT
			&& ((Unit*)entities[i])->isDead)
		{
			for (int j = 0; j < 5; j++)
			{
				auto it = std::find(unitGroups[j].begin(), unitGroups[j].end(), *(entities.begin() + i));
				if (it != unitGroups[j].end())
				{
					unitGroups[j].erase(it);
				}
			}

			deadUnits.push_back((Unit*)*(entities.begin() + i));
			entities.erase(entities.begin() + i);
			printf("deleting entity, size: %d\n", entities.size());
		}
	}

	updateUnits(friendlyUnits, tick);
	updateUnits(enemyUnits, tick);
}

Level::~Level()
{
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

void Level::render(float rot)
{

	
	for (std::vector<Tile*> tileRow : tileGrid)
	{
		for (Tile* tile : tileRow)
		{
			tile->render(rot);
		}
	}

	for (int i = 0; i < deadUnits.size(); i++)
	{
		deadUnits[i]->renderGravestone();
	}

	for (int i = 0; i < enemyUnits.size(); i++)
	{
		enemyUnits[i]->render();
	}

	for (int i = 0; i < friendlyUnits.size(); i++)
	{
		friendlyUnits[i]->render();
	}

	if (!unitGroups[0].empty())
	{
		// draw only the actions of the most recently selected unit
		if (unitGroups[0].size() > 0)
		{
			unitGroups[0].back()->drawActions();
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

const std::vector<std::vector<Tile*>>& Level::getTileGrid()
{
	return tileGrid;
}

const std::vector<Unit*>& Level::getFriendlyUnits()
{
	return friendlyUnits;
}

const std::vector<Unit*>& Level::getEnemyUnits()
{
	return enemyUnits;
}
const std::vector<Structure*>& Level::getStructures()
{
	return structures;
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

			BuildOrRepairAction* buildAction = new BuildOrRepairAction(structure);

			for (Unit* unit : unitGroups[0])
			{
				//unit->target = structure;
				unit->addAction(buildAction, true);
			}
		}

	}
}

void Level::updateUnits(std::vector<Unit*>& units, clock_t& tick)
{
	for (int i = 0; i < units.size(); i++)
	{
		while (i < units.size() && units[i]->isDead)
		{
			if (units[i]->friendly)
			{
				printf("deleting friendly unit, ");
			}
			else
			{
				printf("deleting enemy unit, ");
			}
			deadUnits.push_back(*(units.begin() + i));
			units.erase(units.begin()+i);
			printf("size: %d\n", units.size());
		}

		if (i >= units.size()) break;

		units[i]->update(tick);

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

std::vector<GLObject*> Level::getEntitiesFromCoords(glm::vec3& coords)
{
	std::vector<GLObject*> targetedEntities;
	for (GLObject* entity : entities)
	{
		glm::vec3 distToEntity = entity->getPosition() - coords;
		if (glm::length(distToEntity) < 0.6f)
		{
			targetedEntities.push_back(entity);
		}
	}

	return targetedEntities;
}

void Level::addTarget(glm::vec3& coords, bool modifier)
{
	std::vector<GLObject*> targetedEntities = getEntitiesFromCoords(coords);
	GLObject* target = nullptr;
	for (GLObject* targetedEntity : targetedEntities)
	{
		if ((targetedEntity->OBJECT_TYPE == ObjectType::UNIT
			&& !((Unit*)targetedEntity)->isDead
			&& !((Unit*)targetedEntity)->friendly)
			|| (targetedEntity->OBJECT_TYPE == ObjectType::STRUCTURE
				&& !((Structure*)targetedEntity)->built))
		{
			target = targetedEntity;
			break;
		}
	}

	for (Unit* activeUnit : unitGroups[0])
	{
		if (target)
		{
			Action* targetAction = nullptr;
			if (target->OBJECT_TYPE == ObjectType::UNIT)
			{
				targetAction = new TargetAction((Unit*)target);
			}
			else if (target->OBJECT_TYPE == ObjectType::STRUCTURE)
			{
				targetAction = new BuildOrRepairAction((Structure*)target);
			}
			activeUnit->addAction(targetAction, !modifier);
		}
		else
		{
			MoveAction* moveAction = new MoveAction(coords);
			activeUnit->addAction(moveAction, !modifier);
		}
	}
}

void Level::selectUnit(glm::vec3& coords, bool modifier)
{
	std::vector<GLObject*> targetedEntities = getEntitiesFromCoords(coords);
	Unit* targetedUnit = nullptr;
	for (GLObject* targetedEntity : targetedEntities)
	{
		if (targetedEntity
			&& targetedEntity->OBJECT_TYPE == ObjectType::UNIT
			&& !((Unit*)targetedEntity)->isDead
			&& ((Unit*)targetedEntity)->friendly
			&& std::find(unitGroups[0].begin(), unitGroups[0].end(), targetedEntity) == unitGroups[0].end())
		{
			targetedUnit = (Unit*)targetedEntity;
		}
	}

	if (!modifier)
		unitGroups[0].clear();

	if (targetedUnit)
	{
		/*if (targetedEntity->OBJECT_TYPE == ObjectType::UNIT)
		{*/
			if (targetedUnit->friendly && !targetedUnit->isDead)
			{
				printf("adding unit to selection, size: %d\n", unitGroups[0].size());
				unitGroups[0].push_back(targetedUnit);
			}

			
		//}
		//else if (targetedEntity->OBJECT_TYPE == ObjectType::STRUCTURE)
		//{
		//	// TODO: show range and what not
		//}
	}


}