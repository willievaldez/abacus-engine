#include <Game/Tile.h>

#include <Game/Item.h>
#include <Game/Structure.h>
#include <Game/Unit.h>

#include <glm/gtc/matrix_transform.hpp> // translate
#include <assert.h>
#include <algorithm>


Tile::Tile(std::string token)
{
	OBJECT_TYPE = ObjectType::STRUCTURE;

	if (token == "WR" || token == "W")
	{
		m_asset = GLObject::GLAsset("wallassetright.png");
		m_traversable = false;
	}
	else if (token == "CR")
	{
		m_asset = GLObject::GLAsset("ticornrasset.png");
		m_traversable = false;
	}
	else if (token == "CL")
	{
		m_asset = GLObject::GLAsset("ticornlasset.png");
		m_traversable = false;
	}
	else if (token == "WT")
	{
		m_asset = GLObject::GLAsset("topwallasset.png");
		m_traversable = false;
	}
	else if (token == "WL")
	{
		m_asset = GLObject::GLAsset("wallassetleft.png");
		m_traversable = false;
	}
	else if (token == "0")
	{
		m_traversable = false;
	}
	else
	{
		m_asset = GLObject::GLAsset("floorasset1.png");
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

void Tile::Render(const UniformContainer& uniforms)
{
	if (m_asset) GLObject::Render(uniforms);
}
void Tile::Render(std::vector<GLObject*>& deferredAssets)
{
	UniformContainer uniforms;
	uniforms.AddObject("debugHighlight", m_debugHighlight);
	Render(uniforms);

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

void Tile::AddItem(Item* item)
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

bool Tile::Collision(const glm::vec3& pt, float radius)
{
	return !m_traversable;
}

bool Tile::Collision(const glm::vec3& pt, std::set<Unit*>& hitUnits, float radius)
{
	if (m_traversable)
	{
		// check for collision with units
		float closestHit = -1.0f;
		for (Unit* unit : m_units)
		{
			float dist = glm::length(pt - unit->GetPosition());
			if (dist <= unit->GetMetadata().hitbox_radius + radius)
			{
				if (closestHit < 0.0f || dist < closestHit)
				{
					hitUnits.insert(unit);
				}
			}
		}
	}
	return !m_traversable;
}

void Tile::Interact(Unit* player)
{
	if (m_structure)
	{
		m_structure->Interact(player);
	}

	// TODO: sometimes items will not get used but will get deleted
	for (auto& item : m_items)
	{
		float dist = glm::length(item->GetPosition() - player->GetPosition());
		if (dist <= player->GetMetadata().hitbox_radius + 0.5f) // TODO hard coded mana orb size
		{
			player->TakeDamage(-20.0f); // regain mana
			delete item;
		}
	}

	m_items.clear();
}

void Tile::SetDebugHighlight(const glm::vec3& color)
{
	m_debugHighlight = color;
}
