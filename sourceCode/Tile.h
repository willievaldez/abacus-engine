#pragma once

#include "GLObject.h"
#include "Spawner.h"
#include "Turret.h"

class Tile : public GLObject
{
public:
	Tile(std::string);
	~Tile();

	void render() override;
	void setColor(glm::vec3);
	void addStructure(Structure*);
	bool traversable;

private:
	Structure* structure;
};