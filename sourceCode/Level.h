#pragma once

#include <glad/glad.h>
#include <vector>

#include "Unit.h"
#include "Tile.h"



class Level
{
public:
	Level(const char*);
	~Level();

	bool getTileFromCoords(glm::vec3, std::pair<int, int>&);
	bool getCoordsFromTile(std::pair<int, int>, glm::vec3&);

	glm::vec3 getSpawn();
	int addEntity(Unit*);
	void buildStructure(GLObject*);
	void moveEntities();
	void moveEntity(int, glm::vec3);
	void moveEntityToDestination(Unit*);
	void render(GLuint);
	void reload();
	Unit* selectUnit(glm::vec3&);
private:
	std::vector<std::vector<Tile*>> tileGrid;
	std::vector<Unit*> entities;
	glm::vec3 spawn;
	std::string filepath;

	void makeLevelFromFile();
};