#include "Spawner.h"

Spawner::Spawner(glm::vec3& pos, bool friendlySpawner) : Structure(pos, 0)
{
	STRUCTURE_TYPE = StructureType::SPAWNER;
	friendly = friendlySpawner;
	if (friendly)
	{
		textureID = GLObject::Asset("AstroChurch.png");
		spawnedTexture = GLObject::Asset("botboi.png");
	}
	else
	{
		built = true;
		health = 100.0f;
		textureID = GLObject::Asset("pentagram.png");
		spawnedTexture = GLObject::Asset("demongrunt.png");
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

		unit = new Unit(spawnedTexture);
		unit->friendly = friendly;
		glm::vec3 spawnPos = position + glm::vec3(-0.5f, 0.5f, 0.0f);
		unit->setPosition(spawnPos);
	}
	return unit;
}