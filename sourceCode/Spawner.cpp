#include "Spawner.h"

Spawner::Spawner(glm::vec3& pos, Asset* asset) : Structure(pos, asset)
{
	STRUCTURE_TYPE = StructureType::SPAWNER;
	if (asset == GLObject::GLAsset("AstroChurch.png"))
	{
		friendly = true;
		spawnAsset = GLObject::GLAsset("botboi.png");
		period = 10000;
	}
	else
	{
		friendly = false;
		built = true;
		health = 100.0f;
		spawnAsset = GLObject::GLAsset("demongrunt.png");
		period = 20000;
	}
}

Spawner::~Spawner()
{

}

Unit* Spawner::spawn(clock_t time)
{
	Unit* unit = nullptr;
	float elapsedTime = (time - lastUpdateTime) / CLOCKS_PER_SEC;
	if (time - lastUpdateTime >= period)
	{
		lastUpdateTime = time;

		unit = new Unit(spawnAsset, friendly);
		glm::vec3 spawnPos = position + glm::vec3(-0.5f, 0.5f, 0.0f);
		unit->setPosition(spawnPos);
	}
	return unit;
}