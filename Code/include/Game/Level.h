#pragma once

#include <vector>

#include <Game/Unit.h>
#include <Game/Tile.h>

#include <Utility/KeyMap.h>

class Tile;
class Unit;
class Player;
struct Camera;

class PointLight;

enum class LevelState
{
	RUNNING,
	WON,
	LOST
};

class Level
{
public:
	Level(const char* file);
	~Level();

	static Level* Get();
	void SetLevelState(LevelState);

	Tile* GetTileFromCoords(const glm::vec3&);
	std::vector<Tile*> GetTilesFromCoords(const glm::vec3&, float radius);
	bool getCoordsFromTile(std::pair<int, int>, glm::vec3&);

	void BasicAttack(const glm::vec3& direction);

	void Update(const clock_t& tick, const KeyMap& keyMap);

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

	LevelState m_levelState = LevelState::RUNNING;

	void MakeLevelFromFile();
};