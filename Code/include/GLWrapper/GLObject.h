#pragma once

#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp> // glm::mat4, glm::transform, glm::length

#include <GLWrapper/Asset.h>
#include <GLWrapper/Model.h>

enum class ObjectType
{
	GENERIC,
	UNIT,
	STRUCTURE
};

class GLObject
{

public:

	ObjectType OBJECT_TYPE;
	int m_entityId = -1;

	GLObject();
	GLObject(const char*, const AssimpParsingParams& = {});
	GLObject(Asset*);
	~GLObject();

	virtual void Render();
	virtual void Render(const UniformContainer& uniforms);

	const glm::vec3& GetPosition();
	virtual void SetPosition(const glm::vec3&);
	const glm::vec3& GetDirection();
	void SetDirection(const glm::vec3& direction, bool normalize=true);

	static void useShaderProgram(const glm::mat4&, const glm::mat4&, const glm::vec3&);
	static void Initialize();
	static void releaseBuffers();
	static Asset* GLAsset(const char*);

protected:
	//FMOD::Sound* moveSound;
	glm::vec3 m_direction = glm::vec3(0.0f, -1.0f, 0.0f);
	glm::vec3 m_position = glm::vec3(0.0f);
	Asset* m_asset = nullptr;
	Model* m_model = nullptr;

	static std::unordered_map<std::string, Asset*> s_assets;
};