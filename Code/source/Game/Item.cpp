#include <Game/Item.h>
#include <Game/Level.h>
#include <Game/Player.h>
#include <GLWrapper/PointLight.h>

Item* Item::Create(const std::string& type, const glm::vec3& pos)
{
	Item* item = new Item(type, pos);

	Tile* tile = Level::Get()->GetTileFromCoords(pos);
	if (tile)
	{
		tile->AddItem(item);
	}

	return item;
}

Item::~Item()
{
	if (m_light)
	{
		delete m_light;
	}
}

Item::Item(const std::string& type, const glm::vec3& pos) : GLObject(type.c_str())
{
	SetPosition(pos);
	m_light = PointLight::Create(pos, 0.5f, 2.0f);
}