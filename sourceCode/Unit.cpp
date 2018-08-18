#include "Unit.h"

#include <glm/gtc/matrix_transform.hpp> // translate


Unit::Unit(const char* asset) : GLObject(asset) {
	health = 100.0f;
}

Unit::~Unit() {}

void Unit::render(GLuint& shaderProgram)
{
	GLObject::render(shaderProgram);

	glBindVertexArray(VAO);

	for (glm::vec3 destination : destinations)
	{
		glm::mat4 toWorld = glm::scale(glm::translate(glm::mat4(1.0f), destination), glm::vec3(0.35f, 0.35f, 0.35f));
		GLuint matrixid = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);

		GLuint texBool = glGetUniformLocation(shaderProgram, "useTex");
		glUniform1i(texBool, true);

		glBindTexture(GL_TEXTURE_2D, GLObject::addAsset("pixelflag.png"));

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	drawHealthBar(shaderProgram);

	glBindVertexArray(0);

}

void Unit::drawHealthBar(GLuint& shaderProgram)
{
	GLuint matrixid = glGetUniformLocation(shaderProgram, "model");
	GLuint texBool = glGetUniformLocation(shaderProgram, "useTex");
	GLuint colorId = glGetUniformLocation(shaderProgram, "color");

	glUniform1i(texBool, false);


	glm::vec3 healthBarPosition(position.x, position.y + GLObject::tileSize / 1.9f, position.z);

	glm::mat4 toWorld = glm::scale(glm::translate(glm::mat4(1.0f), healthBarPosition), glm::vec3(0.9f, 0.05f, 1.0f));
	glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);
	glm::vec3 red(1.0f, 0.0f, 0.0f);
	glUniform3fv(colorId, 1, &red[0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


	toWorld = glm::scale(glm::translate(glm::mat4(1.0f), healthBarPosition), glm::vec3(0.9, 0.05f * (health/100.0f), 1.0f));
	glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);
	glm::vec3 green(0.0f, 1.0f, 0.0f);
	glUniform3fv(colorId, 1, &green[0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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

bool Unit::takeDamage(float dmg)
{
	health -= dmg;

	if (health <= 0.0f)
	{
		return true;
	}

	return false;
}