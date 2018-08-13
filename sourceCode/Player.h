#pragma once

#include "Unit.h"
#include "Follower.h"

class Player : public Unit
{
public:

private:
	std::vector<Follower*> followers;
};