#pragma once


#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp> // glm::mat4, glm::transform, glm::length

#include "Asset.h"

#define INSTALL_DIR std::string("../../")

enum ObjectType
{
	GENERIC,
	UNIT,
	STRUCTURE
};

enum State
{
	IDLE,
	MOVING,
	ATTACKING
};


class GLObject
{

public:

	ObjectType OBJECT_TYPE;
	int entityId;

	GLObject();
	GLObject(const char*);
	GLObject(Asset*);
	~GLObject();

	virtual void draw();

	glm::vec3 getPosition();
	void setPosition(glm::vec3&);

	const State& getState();
	void setState(State);

	static void useShaderProgram(glm::mat4&, glm::mat4&);
	static void initialize();
	static void releaseBuffers();
	static Asset* GLAsset(const char*);
	static void setIsometricSkew(float, float);
	static const glm::mat4 getIsometricSkew();
	static void drawDebugLine(glm::vec3, glm::vec3);

protected:
	//FMOD::Sound* moveSound;
	glm::vec3 position;
	State currentState;
	Asset* asset;

	static std::unordered_map<std::string, Asset*> assets;
};