#include "Spawner.h"

Spawner::Spawner(glm::vec3& pos)
{
	position = pos;
	lastUpdateTime = clock();
	period = 10000;
}

Spawner::~Spawner()
{

}

Unit* Spawner::spawn(clock_t time)
{
	Unit* newEnemy = nullptr;
	float elapsedTime = (time - lastUpdateTime) / CLOCKS_PER_SEC;
	if (time - lastUpdateTime >= period)
	{
		lastUpdateTime = time;

		newEnemy = new Unit(position);
	}
	return newEnemy;
}