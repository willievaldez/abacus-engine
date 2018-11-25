#pragma once

#include "GLObject.h"
#include "Spawner.h"
#include "Turret.h"

class Tile : public GLObject
{
public:
	Tile(std::string);
	~Tile();

	//void renderFloor();
	//void renderStructure();

	void draw() override;
	//void render(float);
	void addStructure(Structure*);
	bool traversable;

private:
	Structure* structure;
};