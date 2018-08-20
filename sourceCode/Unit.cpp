#include "Unit.h"

#include <glm/gtc/matrix_transform.hpp> // translate


Unit::Unit(const char* asset) : GLObject(asset) 
{
	OBJECT_TYPE = ObjectType::UNIT;
	health = 100.0f;
	speed = 0.15f;
	friendly = true;
	isDead = false;
}

Unit::Unit(glm::vec3& pos)
{
	OBJECT_TYPE = ObjectType::UNIT;
	health = 100.0f;
	speed = 0.15f;
	color = glm::vec3(0.5f, 0.0f, 0.0f);
	renderTexture = false;
	position = pos;
	friendly = false;
	isDead = false;
}

Unit::~Unit() {}

void Unit::render(GLuint& shaderProgram)
{
	if (isDead)
	{
		glBindVertexArray(VAO);

		glm::mat4 toWorld = glm::translate(glm::mat4(1.0f), position);
		GLuint matrixid = glGetUniformLocation(shaderProgram, "model");
		glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);

		GLuint texBool = glGetUniformLocation(shaderProgram, "useTex");
		glUniform1i(texBool, true);

		glBindTexture(GL_TEXTURE_2D, GLObject::Asset("gravestone.png"));

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
	}

	else
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

			glBindTexture(GL_TEXTURE_2D, GLObject::Asset("pixelflag.png"));

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}

		drawHealthBar(shaderProgram);

		glBindVertexArray(0);
	}


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

	healthBarPosition.x -= 0.9f - (0.9f * (health / 100.0f));
	toWorld = glm::scale(glm::translate(glm::mat4(1.0f), healthBarPosition), glm::vec3(0.9f * (health / 100.0f), 0.05f, 1.0f));
	glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);
	glm::vec3 green(0.0f, 1.0f, 0.0f);
	glUniform3fv(colorId, 1, &green[0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Unit::drawSelectedMarker(GLuint& shaderProgram)
{
	glBindVertexArray(VAO);

	glm::mat4 toWorld = glm::scale(glm::translate(glm::mat4(1.0f), position), glm::vec3(0.9f, 0.9f, 1.0f));
	GLuint matrixid = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);

	GLuint texBool = glGetUniformLocation(shaderProgram, "useTex");
	glUniform1i(texBool, true);

	if (friendly)
	{
		glBindTexture(GL_TEXTURE_2D, GLObject::Asset("greentarget.png"));
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, GLObject::Asset("redtarget.png"));
	}

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void Unit::targetNearestEntity(std::vector<Unit*> entities)
{
	float dist = 10000.0f;
	for (Unit* entity : entities)
	{
		if (this == entity || entity->friendly == this->friendly || entity->isDead) continue;
		float distToEntity = glm::length(entity->getPosition() - getPosition());
		if (distToEntity < dist)
		{
			dist = distToEntity;
			target = entity;
		}
	}
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
	target = nullptr;
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
		isDead = true;
		health = 0.0f;
		return true;
	}

	return false;
}