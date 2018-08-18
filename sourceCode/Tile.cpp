#include "Tile.h"

#include <glm/gtc/matrix_transform.hpp> // translate


Tile::Tile(std::string token)
{
	if (token == "W")
	{
		renderTexture = false;
		traversable = false;
		color = glm::vec3(0.0f);
	}
	else if (token == "Summoning Circle")
	{
		renderTexture = true;
		traversable = true;
		color = glm::vec3(1.0f);
		textureID = GLObject::addAsset("pentagram.png");
	}
	else
	{
		renderTexture = false;
		traversable = true;
		color = glm::vec3(1.0f);
	}

}

Tile::~Tile()
{

}

void Tile::render(GLuint& shaderProgram)
{
	glm::mat4 toWorld = glm::translate(glm::mat4(1.0f), position);

	glBindVertexArray(VAO);

	GLuint matrixid = glGetUniformLocation(shaderProgram, "model");
	glUniformMatrix4fv(matrixid, 1, GL_FALSE, &toWorld[0][0]);


	GLuint texBool = glGetUniformLocation(shaderProgram, "useTex");
	glUniform1i(texBool, false);
	GLuint colorId = glGetUniformLocation(shaderProgram, "color");
	glUniform3fv(colorId, 1, &color[0]);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	if (renderTexture)
	{
		glUniform1i(texBool, true);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);
	glUniform1i(texBool, false);
}

void Tile::setColor(glm::vec3 newColor)
{
	color = newColor;
}

void Tile::setAsset(GLint texID)
{
	renderTexture = true;
	textureID = texID;
}
