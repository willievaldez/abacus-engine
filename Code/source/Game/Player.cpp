#include <Game/Player.h>

#include <Game/Unit.h>
#include <Game/Level.h>

#include <GLWrapper/PointLight.h>

Player::Player(const glm::vec3& pos)
{
	m_unit = Unit::Create("Lumaton");
	m_unit->SetPosition(pos);
	m_light = PointLight::Create(pos);
	m_light->ambientRadius = 10.0f;
	m_light->radius = m_unit->GetHealth() / 10.0f;
}

void Player::Update(const clock_t& tick)
{
	m_unit->Update(tick);

	m_light->pos = m_unit->GetPosition();
	m_light->radius = m_unit->GetHealth() / 10.0f;
}