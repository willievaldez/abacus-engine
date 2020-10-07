#pragma once

#include <unordered_map>
#include <memory>


#include <GLWrapper/GLObject.h>
#include <Game/Spawner.h>
#include <Game/Turret.h>

class Tile : public GLObject
{
public:
	Tile(std::string);
	Tile() {};
	~Tile();


	//void renderFloor();
	//void renderStructure();

	void Update(clock_t);
	void Render() override;
	void AddStructure(Structure*);
	bool Collision(const glm::vec3& pt);

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

	static TileTypeMap& AccessTileTypes();
};

#define REGISTER_TILE_TYPE(TYPE_NAME, CLASS) size_t g_tileNum##__COUNTER__ = Tile::RegisterTileType<CLASS>(TYPE_NAME);
