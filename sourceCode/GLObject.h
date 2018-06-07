#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include "Model.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/string_cast.hpp>
#include <vector>

class GLObject
{
public:
	GLObject(std::vector<GLuint>, std::vector<glm::vec3>, std::vector<glm::vec3>);
	GLObject(const char*);
	~GLObject();

	void render(GLuint);
	void move(glm::vec3);

private:
	//FMOD::Sound* moveSound;
	GLsizei number_of_indices;
	Model* model;
	glm::vec3 position;
	GLuint VBO, VAO, EBO;

};