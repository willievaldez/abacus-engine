#include "Tile.h"

#include <glm/gtc/matrix_transform.hpp> // translate

int rand_4 = 0;

Tile::Tile(std::string token)
{
	OBJECT_TYPE = ObjectType::STRUCTURE;

	if (token == "W")
	{
		asset = GLObject::GLAsset("isogrid.png");
		traversable = false;
	}
	else
	{
		std::string randomFloorTileName = "earth_tile_" + std::to_string(rand_4++ + 1) + ".png";
		asset = GLObject::GLAsset(randomFloorTileName.c_str());
		if (rand_4 >= 4)
		{
			rand_4 = 0;
		}
		traversable = true;
	}

}

Tile::~Tile()
{

}

void Tile::draw()
{
	if (asset) GLObject::draw();
}

//void Tile::renderFloor()
//{
//	glm::mat4 toWorld = GLObject::isometricSkew * glm::translate(glm::mat4(1.0f), position);
//
//	glBindVertexArray(VAO);
//
//	GLuint matrixid = glGetUniformLocation(shaderProgram, "model");
//	glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);
//
//
//	GLuint texBool = glGetUniformLocation(shaderProgram, "useTex");
//	glUniform1i(texBool, false);
//	GLuint colorId = glGetUniformLocation(shaderProgram, "color");
//	glUniform3fv(colorId, 1, &color[0]);
//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//
//	if (renderTexture)
//	{
//		glUniform1i(texBool, true);
//		glBindTexture(GL_TEXTURE_2D, textureID);
//		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//	}
//
//	glBindVertexArray(0);
//	glUniform1i(texBool, false);
//}
//
//void Tile::renderStructure()
//{
//	if (structure)
//	{
//		structure->render();
//	}
//}
//
//void Tile::render(float debugFloat)
//{
//	glm::mat4 toWorld = GLObject::isometricSkew * glm::translate(glm::mat4(1.0f), position);
//
//	glBindVertexArray(VAO);
//
//	GLuint matrixid = glGetUniformLocation(shaderProgram, "model");
//	glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);
//
//
//	GLuint texBool = glGetUniformLocation(shaderProgram, "useTex");
//	glUniform1i(texBool, false);
//	GLuint colorId = glGetUniformLocation(shaderProgram, "color");
//	glUniform3fv(colorId, 1, &color[0]);
//	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//
//	if (renderTexture)
//	{
//		glUniform1i(texBool, true);
//		glBindTexture(GL_TEXTURE_2D, textureID);
//		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
//	}
//
//	glBindVertexArray(0);
//	glUniform1i(texBool, false);
//
//	if (structure)
//	{
//		structure->render();
//	}
//}

void Tile::addStructure(Structure* newStructure)
{
	structure = newStructure;
}
