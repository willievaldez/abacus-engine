#include "Unit.h"

#include <glm/gtc/matrix_transform.hpp> // translate

Unit::Unit(Asset* asset, bool isFriendly) : GLObject(asset)
{
	OBJECT_TYPE = ObjectType::UNIT;
	health = 100.0f;
	speed = 0.15f;
	friendly = isFriendly;
	isDead = false;

	if (friendly)
	{
		idleAction = new IdleDefendAction(10.0f);
	}
	else
	{
		idleAction = new IdleAttackAction();
	}

}

Unit::Unit(const char* asset, bool isFriendly) : Unit(GLObject::GLAsset(asset), isFriendly) {}

Unit::Unit(glm::vec3& pos, bool isFriendly)
{
	OBJECT_TYPE = ObjectType::UNIT;
	health = 100.0f;
	speed = 0.15f;
	position = pos;
	friendly = isFriendly;
	isDead = false;

	if (friendly)
	{
		idleAction = new IdleDefendAction(10.0f);
	}
	else
	{
		idleAction = new IdleAttackAction();
	}

}

Unit::~Unit() {
	Action* actionToDelete = nullptr;
	while (currentAction)
	{
		actionToDelete = currentAction;
		currentAction = currentAction->nextAction;
		delete actionToDelete;
	}
	delete idleAction;
}

void Unit::draw()
{
	if (currentState == State::IDLE)
	{
		GLObject::draw();
	}
	else if (currentState == State::MOVING)
	{
		// TODO: render moving animation here
		GLObject::draw();
	}
	//drawHealthBar();
}


//void Unit::drawHealthBar()
//{
//
//	glBindVertexArray(VAO);
//
//
//	GLuint matrixid = glGetUniformLocation(shaderProgram, "model");
//	GLuint texBool = glGetUniformLocation(shaderProgram, "useTex");
//	GLuint colorId = glGetUniformLocation(shaderProgram, "color");
//
//	glUniform1i(texBool, false);
//
//
//
//	glm::vec4 isometricPosition = GLObject::isometricSkew * glm::vec4(position, 1.0f);
//	glm::vec3 healthBarPosition(isometricPosition.x, isometricPosition.y + GLObject::tileSize / 1.9f, isometricPosition.z);
//
//	glm::mat4 toWorld = glm::scale(glm::translate(glm::mat4(1.0f), healthBarPosition), glm::vec3(0.9f, 0.05f, 1.0f));
//	glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);
//	glm::vec3 red(1.0f, 0.0f, 0.0f);
//	glUniform3fv(colorId, 1, &red[0]);
//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//
//	healthBarPosition.x -= 0.9f - (0.9f * (health / 100.0f));
//	toWorld = glm::scale(glm::translate(glm::mat4(1.0f), healthBarPosition), glm::vec3(0.9f * (health / 100.0f), 0.05f, 1.0f));
//	glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);
//	glm::vec3 green(0.0f, 1.0f, 0.0f);
//	glUniform3fv(colorId, 1, &green[0]);
//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//
//	glBindVertexArray(0);
//
//}

void Unit::update(clock_t tick)
{
	if (!currentAction) idleAction->execute(tick, this);

	else if (currentAction->execute(tick, this))
	{
		Action* actionToDelete = currentAction;
		currentAction = currentAction->nextAction;
		delete actionToDelete;
	}
}

void Unit::addAction(Action* action, bool clearActions)
{
	if (clearActions)
	{
		Action* actionToDelete = nullptr;
		Action* cachedAction = currentAction;
		while (cachedAction && cachedAction != idleAction)
		{
			actionToDelete = cachedAction;
			cachedAction = cachedAction->nextAction;
			delete actionToDelete;
		}
		idleAction->nextAction = nullptr;
		currentAction = nullptr;
	}

	if (!currentAction) currentAction = action;
	else 
	{
		Action* traversalAction = currentAction;
		while (traversalAction->nextAction)
		{
			traversalAction = traversalAction->nextAction;
		}

		traversalAction->nextAction = action;
	}
}

void Unit::drawActions()
{
	Action* actionToDraw = currentAction;
	while (actionToDraw)
	{
		actionToDraw->draw(); // TODO: figure out crash here
		actionToDraw = actionToDraw->nextAction;
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