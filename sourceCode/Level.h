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
	int addEntity(Unit*);
	Structure* placeStructure(GLObject*);
	void moveEntities();
	void moveEntity(int, glm::vec3);
	void moveEntityToDestination(Unit*);
	void moveEntityToTarget(Unit*);
	void render(GLuint);
	void reload();
	Unit* selectUnit(glm::vec3&);
private:
	std::vector<std::vector<Tile*>> tileGrid;
	std::vector<Unit*> entities;
	std::vector<Spawner*> spawners;
	std::vector<Structure*> structures;
	glm::vec3 spawn;
	std::string filepath;
	clock_t tickTime;

	void makeLevelFromFile();
};