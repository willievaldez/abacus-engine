#include "Spawner.h"

Spawner::Spawner(glm::vec3& pos)
{
	position = pos;
	lastUpdateTime = clock();
	period = std::chrono::milliseconds(5000);
}

Spawner::~Spawner()
{

}

Enemy* Spawner::spawn(clock_t time)
{
	//if (time - lastUpdateTime >= period)
	//{

	//}
	return nullptr;
}