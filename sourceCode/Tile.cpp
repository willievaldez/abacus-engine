#include "Tile.h"


Tile::Tile(std::string token, int xOffset, int yOffset)
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

	renderTexture = false;

	x = xOffset;
	y = yOffset;
}

Tile::~Tile()
{

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
