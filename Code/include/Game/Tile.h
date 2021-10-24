#pragma once

#include <unordered_map>
#include <memory>
#include <set>

#include <GLWrapper/GLObject.h>

class Item;
class Structure;
class Unit;

class Tile : public GLObject
{
public:
	Tile(std::string);
	Tile() {};
	~Tile();

	void Update(clock_t);
	void Render(const UniformContainer&) override;
	void Render(std::vector<GLObject*>&);
	void AddStructure(Structure*);
	void AddItem(Item*);
	void AddUnit(Unit*);
	void RemoveUnit(Unit*);
	bool Collision(const glm::vec3& pt, float radius = 0.0f);
	bool Collision(const glm::vec3& pt, std::set<Unit*>& hitUnits, float radius = 0.0f);
	void Interact(Unit* player);
	void SetDebugHighlight(const glm::vec3&);

	template<typename T>
	static size_t RegisterTileType(const char* typeName)
	{
		//  "duplicate Tiles with name " + typeName
		auto foundType = AccessTileTypes().find(typeName);
		assert(foundType == AccessTileTypes().end());
		AccessTileTypes()[typeName] = &std::make_shared<T>;
		return AccessTileTypes().size();
	};

	static std::shared_ptr<Tile> CreateTile(const char* typeName);
	using TileTypeMap = std::unordered_map<std::string, std::shared_ptr<Tile>(*)()>;

private:
	Structure* m_structure = nullptr;
	bool m_traversable = false;
	std::vector<Item*> m_items;
	std::vector<Unit*> m_units;
	glm::vec3 m_debugHighlight = glm::vec3(0.0f);

	static TileTypeMap& AccessTileTypes();
};

#define REGISTER_TILE_TYPE(TYPE_NAME, CLASS) size_t g_tileNum##__COUNTER__ = Tile::RegisterTileType<CLASS>(TYPE_NAME);
