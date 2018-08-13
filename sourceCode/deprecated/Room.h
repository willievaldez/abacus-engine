#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <string>
#include <vector>

class Room
{
public:
	std::vector<glm::vec3> border;
	glm::vec3 center;


	Room(const char*, glm::vec3);
	Room(Room*, glm::vec3);
	~Room();

	void render(GLuint);
	void collidesWithBorder(glm::vec3, glm::vec3&);
	bool smartPath(const glm::vec3&, glm::vec3&);
	bool isWithinBounds(glm::vec3);

private:
	GLuint VBO, VAO, lineVBO, lineVAO;
	glm::vec2 minBound, maxBound;
	bool isCopy;

	void draw_debug_line(glm::vec3, glm::vec3, GLuint);

};