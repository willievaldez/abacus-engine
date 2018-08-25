#include "Unit.h"

#include <glm/gtc/matrix_transform.hpp> // translate

Unit::Unit(GLint texID) : GLObject(texID)
{
	OBJECT_TYPE = ObjectType::UNIT;
	health = 100.0f;
	speed = 0.15f;
	friendly = true;
	isDead = false;
}

Unit::Unit(const char* asset) : Unit(GLObject::Asset(asset)) {}

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

void Unit::render()
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
		GLObject::render();

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

		drawHealthBar();

		glBindVertexArray(0);
	}


}

void Unit::drawHealthBar()
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

void Unit::targetNearestEntity(std::vector<GLObject*> entities, bool targetStructure)
{
	float dist = 10000.0f;
	for (GLObject* entity : entities)
	{
		if (targetStructure && entity->OBJECT_TYPE == ObjectType::STRUCTURE)
		{
			float distToEntity = glm::length(entity->getPosition() - getPosition());
			if (distToEntity < dist)
			{
				dist = distToEntity;
				target = entity;
			}
		}
		else if (entity->OBJECT_TYPE == ObjectType::UNIT)
		{
			if (this == entity || ((Unit*)entity)->friendly == this->friendly || ((Unit*)entity)->isDead) continue;
			float distToEntity = glm::length(entity->getPosition() - getPosition());
			if (distToEntity < dist)
			{
				dist = distToEntity;
				target = entity;
			}
		}

	}
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