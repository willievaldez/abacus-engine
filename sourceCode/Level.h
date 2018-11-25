#pragma once

#include <vector>

#include "Unit.h"
#include "Tile.h"
#include "Structure.h"

class Tile;
class Unit;

class Level
{
public:
	Level(const char*);
	~Level();

	bool getTileFromCoords(glm::vec3, std::pair<int, int>&);
	bool getCoordsFromTile(std::pair<int, int>, glm::vec3&);

	void update(clock_t&);
	glm::vec3 getSpawn();
	const std::vector<std::vector<Tile*>>& getTileGrid();
	const std::vector<Unit*>& getFriendlyUnits();
	const std::vector<Unit*>& getEnemyUnits();
	const std::vector<Structure*>& getStructures();


	int addUnit(Unit*);
	void updateUnits(std::vector<Unit*>& units, clock_t&);

	void render(float, float);
	void reload();
	void addTarget(glm::vec3&, bool);
	void selectUnit(glm::vec3&, bool);
	std::vector<GLObject*> getEntitiesFromCoords(glm::vec3&);
	void placeStructure(glm::vec3&, Asset*);

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