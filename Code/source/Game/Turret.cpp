#include <Game/Turret.h>

Turret::Turret(glm::vec3& pos) : Structure(pos, GLObject::GLAsset("turret.png"))
{
	//STRUCTURE_TYPE = StructureType::TURRET;

	range = 5.0f;
	power = 0.3f;
}

Turret::~Turret()
{

}


void Turret::damageEnemyWithinRange(std::vector<Unit*> entities)
{
	for (Unit* entity : entities)
	{
		//if (!entity->friendly && glm::length(entity->GetPosition() - m_position) <= range)
		//{
		//	entity->TakeDamage(power);
		//}
	}
}