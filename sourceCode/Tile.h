#pragma once

#include "GLObject.h"
#include <glm/glm.hpp> //vec2, vec3
#include <unordered_map>

class Tile
{
public:
	Tile(std::string);
	~Tile();

	static void setTileSize(float);
	static void releaseBuffers();
	void render(GLuint);
	void setColor(glm::vec3);

	bool traversable;

private:
	static std::unordered_map<std::string, GLObject*> assets;
	static GLuint VBO, VAO, EBO;

	GLObject* asset;
	glm::vec2 center;
	glm::vec3 color;
};