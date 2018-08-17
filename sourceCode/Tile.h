#pragma once

#include "GLObject.h"

class Tile : public GLObject
{
public:
	Tile(std::string, int, int);
	~Tile();

	void setColor(glm::vec3);
	void setAsset(GLint);
	bool traversable;

	int x, y;

};