#include "Tile.h"

std::unordered_map<std::string, GLObject*> Tile::assets;
GLuint Tile::VAO, Tile::VBO, Tile::EBO;

Tile::Tile(std::string token)
{
	if (token == "W")
	{
		traversable = false;
		color = glm::vec3(0.0f);
	}
	else
	{
		traversable = true;
		color = glm::vec3(1.0f);
	}
}

Tile::~Tile()
{

}

void Tile::setTileSize(float tileSize)
{
	float tilePt = tileSize / 2.0f;

	std::vector<glm::vec3> tilePoints = { glm::vec3(-tilePt,tilePt, 0.0f), glm::vec3(tilePt,tilePt, 0.0f), glm::vec3(tilePt,-tilePt, 0.0f), glm::vec3(-tilePt,-tilePt, 0.0f) };
	std::vector<GLuint> tileIndices = { 0, 1, 2, 0, 2, 3 };

	glGenVertexArrays(1, &Tile::VAO);
	glGenBuffers(1, &Tile::VBO);
	glGenBuffers(1, &Tile::EBO);

	glBindVertexArray(Tile::VAO);

	glBindBuffer(GL_ARRAY_BUFFER, Tile::VBO);
	glBufferData(GL_ARRAY_BUFFER, tilePoints.size() * sizeof(glm::vec3), &tilePoints.front(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Tile::EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, tileIndices.size() * sizeof(GLuint), &tileIndices.front(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void Tile::releaseBuffers()
{
	glDeleteVertexArrays(1, &Tile::VAO);
	glDeleteBuffers(1, &Tile::VBO);
	glDeleteBuffers(1, &Tile::EBO);
}

void Tile::render(GLuint shaderProgram)
{
	GLuint colorId = glGetUniformLocation(shaderProgram, "color");
	glUniform3fv(colorId, 1, &color[0]);

	glBindVertexArray(Tile::VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Tile::setColor(glm::vec3 newColor)
{
	color = newColor;
}