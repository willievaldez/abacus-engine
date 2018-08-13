#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <vector>

class GLObject
{
public:
	int entityId;
	glm::vec3 color;

	GLObject(std::vector<GLuint>&, std::vector<glm::vec3>&);
	~GLObject();

	virtual void render(GLuint&);
	
	glm::vec3 getPosition();
	void setPosition(glm::vec3&);

protected:
	//FMOD::Sound* moveSound;
	GLsizei number_of_indices;
	
	GLuint VBO, VAO, EBO;
	glm::vec3 position;

};