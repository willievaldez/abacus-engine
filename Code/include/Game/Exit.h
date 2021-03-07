#pragma once

#include <Game/Structure.h>

class Exit : public Structure
{
public:
	Exit(const glm::vec3&, Asset*);
	~Exit();

	void Interact(Unit* player) override;

};