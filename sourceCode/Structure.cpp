#include "Structure.h"

#include <glm/gtc/matrix_transform.hpp> // length


Structure::Structure(glm::vec3& pos) : GLObject("botboi.png")
{
	OBJECT_TYPE = ObjectType::STRUCTURE;
	built = false;
	health = 0.0f;
	range = 5.0f;
	power = 0.3f;

	position = pos;
}

Structure::~Structure()
{

}

void Structure::build(float progress)
{
	health += progress;
	if (health >= 100.0f)
	{
		health = 100.0f;
		built = true;
	}
}

void Structure::render(GLuint& shaderProgram)
{
	if (!built)
	{
		drawHealthBar(shaderProgram);
	}
	else
	{
		GLObject::render(shaderProgram);
	}


}

void Structure::drawHealthBar(GLuint& shaderProgram)
{
	GLuint matrixid = glGetUniformLocation(shaderProgram, "model");
	GLuint texBool = glGetUniformLocation(shaderProgram, "useTex");
	GLuint colorId = glGetUniformLocation(shaderProgram, "color");


	glBindVertexArray(VAO);

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

	glBindVertexArray(0);

}

void Structure::damageEnemyWithinRange(std::vector<Unit*> entities)
{
	for (Unit* entity : entities)
	{
		if (!entity->friendly && glm::length(entity->getPosition() - position) <= range)
		{
			entity->takeDamage(power);
		}
	}
}