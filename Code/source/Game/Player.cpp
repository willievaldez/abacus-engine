#include <Game/Player.h>

#include <Game/Unit.h>
#include <Game/Level.h>
#include <GLWrapper/PointLight.h>

PointLight* PointLight::Create(const glm::vec3& p, float i, float r)
{
	PointLight* light = new PointLight(p, i, r);
	Level::Get()->AddLight(light);
	return light;
}

PointLight::~PointLight()
{
	Level::Get()->RemoveLight(this);
}

Player::Player(const glm::vec3& pos)
{
	m_unit = Unit::Create("Lumaton");
	m_unit->SetPosition(pos);
	m_light = PointLight::Create(pos);
}

void Player::Update(const clock_t& tick)
{
	m_unit->Update(tick);

	m_light->pos = m_unit->GetPosition();
	m_light->radius = m_unit->GetHealth() / 15.0f;
}