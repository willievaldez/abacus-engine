#include <Game/Exit.h>

#include <Game/Level.h>

Exit::Exit(const glm::vec3& pos, Asset* asset) : Structure(pos, asset)
{
}

Exit::~Exit()
{
}

void Exit::Interact(Unit* player)
{
	Level::Get()->SetLevelState(LevelState::WON);
}