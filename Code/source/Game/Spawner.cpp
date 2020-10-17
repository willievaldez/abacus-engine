#include <Game/Spawner.h>

Spawner::Spawner(glm::vec3& pos, Asset* asset) : Structure(pos, asset)
{
	m_lastSpawnTime = clock();
	m_entityName = "DemonGrunt";
}

Spawner::~Spawner()
{
	if (m_spawnedUnit)
	{
		delete m_spawnedUnit;
	}
}

void Spawner::Update(clock_t time)
{
	if (!m_firstSpawn)
	{
		m_spawnedUnit = Unit::Create(m_entityName.c_str());
		m_spawnedUnit->SetPosition(m_position);
		m_firstSpawn = true;
	}

	float elapsedSeconds = (time - m_lastSpawnTime) / (float)CLOCKS_PER_SEC;
	if (elapsedSeconds >= m_periodSec && !m_spawnedUnit)
	{
		float distToPlayer = glm::length(Level::Get()->GetPlayerUnit()->GetPosition() - GetPosition());
		if (distToPlayer > 20.0f) // don't respawn if player is near by
		{
			m_spawnedUnit = Unit::Create(m_entityName.c_str());
			m_spawnedUnit->SetPosition(m_position);
		}
	}

	if (m_spawnedUnit && m_spawnedUnit->GetHealth() <= 0.0f)
	{
		m_lastSpawnTime = time;
		delete m_spawnedUnit;
		m_spawnedUnit = nullptr;
	}
}