#include <Game/Tile.h>

#include <glm/gtc/matrix_transform.hpp> // translate
#include <assert.h>
#include <algorithm>


Tile::Tile(std::string token)
{
	OBJECT_TYPE = ObjectType::STRUCTURE;

	if (token == "W")
	{
		m_asset = GLObject::GLAsset("wallassetright.png");
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
}
void Tile::Render(std::vector<GLObject*>& deferredAssets)
{
	Render();
	if (m_structure) deferredAssets.push_back(m_structure);
	for (auto& item : m_items)
	{
		deferredAssets.push_back(item);
	}
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

void Tile::AddItem(GLObject* item)
{
	m_items.push_back(item);
}

void Tile::AddUnit(Unit* unit)
{
	m_units.push_back(unit);
}

void Tile::RemoveUnit(Unit* unit) // TODO: inefficient
{
	for (int i = 0; i < m_units.size(); i++)
	{
		if (unit == m_units[i])
		{
			m_units.erase(m_units.begin() + i);
		}
	}
}

bool Tile::Collision(const glm::vec3& pt, Unit** hitUnit, float radius)
{
	*hitUnit = nullptr;
	if (m_traversable)
	{
		// check for collision with unit (collide with closest unit)
		float closestHit = -1.0f;
		for (Unit* unit : m_units)
		{
			float dist = glm::length(pt - unit->GetPosition());
			if (dist <= unit->GetMetadata().m_radius + radius)
			{
				if (closestHit < 0.0f || dist < closestHit)
				{
					*hitUnit = unit;
				}
			}
		}
	}
	return !m_traversable || *hitUnit;
}

void Tile::Interact(Unit* player)
{
	// TODO: sometimes items will not get used but will get deleted
	for (auto& item : m_items)
	{
		float dist = glm::length(item->GetPosition() - player->GetPosition());
		if (dist <= player->GetMetadata().m_radius + 0.5f) // TODO hard coded mana orb size
		{
			player->TakeDamage(-15.0f); // regain mana
			delete item;
		}
	}

	m_items.clear();
}
