#pragma once

#include <vector>
#include <unordered_set>

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
	INTRO,
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
	void AddPlayer();
	void SetLevelState(LevelState);

	Tile* GetTileFromCoords(const glm::vec3&);
	std::pair<int, int> GetTileIndices(const glm::vec3& pos);
	std::vector<Tile*> GetTilesFromCoords(const glm::vec3& pos, float radius, const glm::vec3& dir = glm::vec3(0.0f));
	bool getCoordsFromTile(std::pair<int, int>, glm::vec3&);

	void BasicAttack(const glm::vec3& direction);

	void Update(const KeyMap& keyMap);

	void Render();
	void Reload();
	Unit* GetPlayerUnit();
	Unit* GetClosestPlayerUnit(const glm::vec3& pos);
	Unit* FindUnit(Unit*);

	int AddUnit(Unit*);
	bool RemoveUnit(Unit*);

	int AddLight(PointLight*);
	bool RemoveLight(PointLight*);

private:
	std::vector<std::vector<Tile*>> m_tileGrid;
	std::unordered_set<Unit*> m_units;
	std::vector<glm::vec3> m_spawns;
	std::string m_filepath;
	clock_t m_tickTime;
	std::vector<Player*> m_players;
	std::vector<PointLight*> m_lightSources;

	LevelState m_levelState = LevelState::INTRO;

	void MakeLevelFromFile();
};