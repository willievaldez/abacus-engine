#include <Game/Spawner.h>

Spawner::Spawner(glm::vec3& pos, Asset* asset) : Structure(pos, asset)
{
	m_lastSpawnTime = clock();
	m_entityName = "DemonGrunt";
}

Spawner::~Spawner()
{

}

void Spawner::Update(clock_t time)
{
	float elapsedSeconds = (time - m_lastSpawnTime) / (float)CLOCKS_PER_SEC;
	if (elapsedSeconds >= m_periodSec && !m_spawnedUnit)
	{
		m_lastSpawnTime = time;

		m_spawnedUnit = Unit::Create(m_entityName.c_str());
		glm::vec3 spawnPos = m_position + glm::vec3(0.0f, 0.0f, 0.0f);
		spawnPos.z = 0.0f;
		m_spawnedUnit->SetPosition(spawnPos);
	}

	if (m_spawnedUnit && m_spawnedUnit->GetHealth() <= 0.0f)
	{
		delete m_spawnedUnit;
		m_spawnedUnit = nullptr;
	}
}