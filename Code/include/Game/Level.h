#pragma once

#include <vector>

#include <Game/Unit.h>
#include <Game/Tile.h>
#include <Game/Structure.h>

class Tile;
class Unit;
struct Camera;

class Level
{
public:
	Level(const char*);
	~Level();

	static Level* Get();

	Tile* GetTileFromCoords(glm::vec3);
	bool getCoordsFromTile(std::pair<int, int>, glm::vec3&);

	void BasicAttack(const glm::vec3&, const glm::vec3&);

	void Update(clock_t& tick, GLFWwindow* window);
	glm::vec3 getSpawn();
	const std::vector<std::vector<Tile*>>& getTileGrid();

	//void updateUnits(std::vector<Unit*>& units, clock_t&);
	//void SetPlayerPosition(Camera& camera, const glm::vec3& moveDir);

	void Render(/*float=0.0f, float=0.0f*/);
	void Reload();
	//void addTarget(glm::vec3&, bool);
	//void selectUnit(glm::vec3&, bool);
	Unit* GetPlayerUnit() { return m_player; };
	std::vector<GLObject*> getEntitiesFromCoords(glm::vec3&);
	void PlaceStructure(glm::vec3&, Asset*);
	int AddUnit(Unit*);
	bool RemoveUnit(Unit*);

private:
	std::vector<std::vector<Tile*>> m_tileGrid;
	std::vector<Unit*> m_units;
	//std::vector<Unit*> m_friendlyUnits;
	//std::vector<Unit*> m_enemyUnits;
	//std::vector<Unit*> m_unitGroups[5];
	//std::vector<Unit*> m_deadUnits;
	//std::vector<Structure*> m_structures;
	glm::vec3 m_spawn;
	std::string m_filepath;
	clock_t m_tickTime;
	Unit* m_player = nullptr;

	GLObject* m_attack = nullptr;
	glm::vec3 m_attackDirection = glm::vec3(0.0f);

	void MakeLevelFromFile();
};