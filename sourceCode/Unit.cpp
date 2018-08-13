#include "Unit.h"

#include <glm/gtc/matrix_transform.hpp> // translate


Unit::Unit(std::vector<GLuint>& indices, std::vector<glm::vec3>& verticies) : GLObject(indices, verticies) {}
Unit::~Unit() {}

void Unit::render(GLuint& shaderProgram)
{
	GLObject::render(shaderProgram);

	glBindVertexArray(VAO);

	for (glm::vec3 destination : destinations)
	{
		glm::mat4 toWorld = glm::scale(glm::translate(glm::mat4(1.0f), destination), glm::vec3(0.5f, 0.5f, 0.5f));
		GLuint matrixid = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);

		GLuint colorId = glGetUniformLocation(shaderProgram, "color");
		glm::vec3 color(1.0f, 0.0f, 0.0f);
		glUniform3fv(colorId, 1, &color[0]);

		glDrawElements(GL_TRIANGLES, number_of_indices, GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);

}

void Unit::addDestination(glm::vec3& dest)
{
	destinations.push_back(dest);
}

void Unit::addToDestinationQueue(glm::vec3& dest)
{
	destinations.insert(destinations.begin(), dest);
}

void Unit::setDestination(glm::vec3 dest)
{
	destinations.clear();
	destinations.push_back(dest);
}

glm::vec3 Unit::getDestination()
{
	return destinations.back();
}

void Unit::popDestination()
{
	destinations.pop_back();
}

bool Unit::hasDestination()
{
	return destinations.size() > 0;
}