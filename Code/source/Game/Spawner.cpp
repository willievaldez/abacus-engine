#include <Game/Spawner.h>

Spawner::Spawner(glm::vec3& pos, Asset* asset) : Structure(pos, asset)
{
	//STRUCTURE_TYPE = StructureType::SPAWNER;
	//if (asset == GLObject::GLAsset("AstroChurch.png"))
	//{
	//	friendly = true;
	//	m_spawnAsset = GLObject::GLAsset("botboi.png");
	//	period = 10000;
	//}
	//else
	//{
	//	friendly = false;
	//	built = true;
	//	health = 100.0f;
		m_spawnAsset = GLObject::GLAsset("demongrunt_run.png");
	//	period = 10000;
	//}
		m_lastSpawnTime = clock();
}

Spawner::~Spawner()
{

}

void Spawner::Update(clock_t time)
{
	Unit* unit = nullptr;
	float elapsedSeconds = (time - m_lastSpawnTime) / (float)CLOCKS_PER_SEC;
	if (elapsedSeconds >= m_periodSec && !m_spawnedUnit)
	{
		m_lastSpawnTime = time;

		m_spawnedUnit = Unit::Create(m_spawnAsset);
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