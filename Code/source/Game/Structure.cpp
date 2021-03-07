#include <Game/Structure.h>
#include <Game/Tile.h>

Structure::Structure(const glm::vec3& pos, Asset* asset) : GLObject(asset)
{
	OBJECT_TYPE = ObjectType::STRUCTURE;
	//STRUCTURE_TYPE = StructureType::GENERIC_TYPE;
	//built = false;
	//health = 0.0f;
	//period = 20000;

	m_position = pos;
}

Structure::~Structure()
{

}

//void Structure::build(float progress)
//{
//	health += progress;
//	if (health >= 100.0f)
//	{
//		health = 100.0f;
//		built = true;
//	}
//}

