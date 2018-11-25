#include "Structure.h"

Structure::Structure(glm::vec3& pos, Asset* asset) : GLObject(asset)
{
	OBJECT_TYPE = ObjectType::STRUCTURE;
	STRUCTURE_TYPE = StructureType::GENERIC_TYPE;
	built = false;
	health = 0.0f;
	lastUpdateTime = clock();
	period = 20000;

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

void Structure::draw()
{
	//if (!built)
	//{
	//	drawHealthBar();
	//}
	//else
	//{
	//	GLObject::draw();
	//}
	GLObject::draw();

}

void Structure::update(clock_t tick)
{
	
}

//void Structure::drawHealthBar()
//{
//	GLuint matrixid = glGetUniformLocation(shaderProgram, "model");
//	GLuint texBool = glGetUniformLocation(shaderProgram, "useTex");
//	GLuint colorId = glGetUniformLocation(shaderProgram, "color");
//
//	glBindVertexArray(VAO);
//
//	glUniform1i(texBool, false);
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
