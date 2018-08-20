#pragma once

#include "GLObject.h"
#include "Structure.h"

class Tile : public GLObject
{
public:
	Tile(std::string);
	~Tile();

	void render(GLuint&) override;
	void setColor(glm::vec3);
	void addStructure(Structure*);
	bool traversable;

private:
	Structure* structure;
};