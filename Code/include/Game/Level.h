#pragma once

#include <vector>

#include <Game/Unit.h>
#include <Game/Tile.h>

class Tile;
class Unit;
class Player;
struct Camera;

class PointLight;

class Level
{
public:
	Level(const char*);
	~Level();

	static Level* Get();

	Tile* GetTileFromCoords(const glm::vec3&);
	std::vector<Tile*> GetTilesFromCoords(const glm::vec3&, float radius);
	bool getCoordsFromTile(std::pair<int, int>, glm::vec3&);

	void BasicAttack(const glm::vec3& direction);

	void Update(const clock_t& tick, GLFWwindow* window);

	void Render();
	void Reload();
	Unit* GetPlayerUnit();

	int AddUnit(Unit*);
	bool RemoveUnit(Unit*);

	int AddLight(PointLight*);
	bool RemoveLight(PointLight*);

private:
	std::vector<std::vector<Tile*>> m_tileGrid;
	std::vector<Unit*> m_units;
	glm::vec3 m_spawn;
	std::string m_filepath;
	clock_t m_tickTime;
	Player* m_player = nullptr;
	std::vector<PointLight*> m_lightSources;

	void MakeLevelFromFile();
};