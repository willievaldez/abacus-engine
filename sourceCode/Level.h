#pragma once

#include <glad/glad.h>
#include <vector>

#include "Unit.h"
#include "Tile.h"
#include "Spawner.h"

class Level
{
public:
	Level(const char*);
	~Level();

	bool getTileFromCoords(glm::vec3, std::pair<int, int>&);
	bool getCoordsFromTile(std::pair<int, int>, glm::vec3&);

	void update(clock_t);
	glm::vec3 getSpawn();
	int addUnit(Unit*);
	void moveUnits(std::vector<Unit*>& units);
	void moveUnitToDestination(Unit*);
	void moveUnitToTarget(Unit*);
	void render();
	void reload();
	void addTarget(glm::vec3&, bool);
	void selectUnit(glm::vec3&, bool);
	std::vector<GLObject*> getEntitiesFromCoords(glm::vec3&);
	void placeStructure(glm::vec3&, GLint);

private:
	std::vector<std::vector<Tile*>> tileGrid;
	std::vector<GLObject*> entities;
	std::vector<Unit*> friendlyUnits;
	std::vector<Unit*> enemyUnits;
	std::vector<Unit*> unitGroups[5];
	std::vector<Unit*> deadUnits;
	std::vector<Structure*> structures;
	glm::vec3 spawn;
	std::string filepath;
	clock_t tickTime;

	void makeLevelFromFile();
};