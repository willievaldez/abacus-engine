#include <Game/Tile.h>

#include <glm/gtc/matrix_transform.hpp> // translate
#include <assert.h>
#include <algorithm>


Tile::Tile(std::string token)
{
	OBJECT_TYPE = ObjectType::STRUCTURE;

	if (token == "W")
	{
		m_asset = GLObject::GLAsset("greentarget.png");
		m_traversable = false;
	}
	else
	{
		m_asset = GLObject::GLAsset("brown.png");
		m_traversable = true;
	}

}

Tile::~Tile()
{

}

void Tile::Update(clock_t tick)
{
	if (m_structure)
	{
		m_structure->Update(tick);
	}
}

void Tile::Render()
{
	if (m_asset) GLObject::Render();
	if (m_structure) m_structure->Render();
}

/*static */
Tile::TileTypeMap& Tile::AccessTileTypes()
{
	static TileTypeMap s_tileTypes;
	return s_tileTypes;
}


/*static*/
std::shared_ptr<Tile> Tile::CreateTile(const char* typeName)
{
	std::shared_ptr<Tile> returnedType = nullptr;
	auto foundType = AccessTileTypes().find(typeName);
	if (foundType != AccessTileTypes().end())
	{
		returnedType = foundType->second();
	}
	return returnedType;
}

void Tile::AddStructure(Structure* newStructure)
{
	m_structure = newStructure;
	glm::vec3 structurePos = GetPosition();
	structurePos.z += 0.0001f;
	m_structure->SetPosition(structurePos);
}

bool Tile::Collision(const glm::vec3& pt)
{
	return !m_traversable;
}
