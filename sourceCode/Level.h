#pragma once

#include <glad/glad.h>
#include <vector>

#include "Unit.h"
#include "Tile.h"

#define INSTALL_DIR std::string("../../")


class Level
{
public:
	Level(const char*);
	~Level();

	glm::vec3 getSpawn();
	int addEntity(Unit*);
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

	bool getTileFromCoords(glm::vec3, std::pair<int, int>&);
};