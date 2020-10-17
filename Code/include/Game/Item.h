#pragma once

#include <GLWrapper/GLObject.h>

class PointLight;

class Item : public GLObject
{
public:
	static Item* Create(const std::string& type, const glm::vec3& pos);
	~Item();
private:
	Item(const std::string& type, const glm::vec3& pos);
	PointLight* m_light;
};