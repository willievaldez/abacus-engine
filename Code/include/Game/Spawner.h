#pragma once

#include <glm/vec3.hpp>

#include <Game/Structure.h>
#include <Game/Unit.h>

class Unit;

class Spawner : public Structure
{
public:
	Spawner(glm::vec3&, Asset*);
	~Spawner();

	void Update(clock_t) override;

private:
	Asset* m_spawnAsset;
	Unit* m_spawnedUnit = nullptr;
	float m_periodSec = 10.0f;
	clock_t m_lastSpawnTime;
};