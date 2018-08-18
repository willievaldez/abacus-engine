#pragma once

#include "GLObject.h"


class Tile : public GLObject
{
public:
	Tile(std::string);
	~Tile();

	void render(GLuint&) override;
	void setColor(glm::vec3);
	void setAsset(GLint);
	bool traversable;

private:

};