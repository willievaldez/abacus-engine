#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <vector>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp> // glm::mat4, glm::transform, glm::length

#define INSTALL_DIR std::string("../../")

enum ObjectType
{
	GENERIC,
	UNIT,
	STRUCTURE
};


class GLObject
{
public:
	ObjectType OBJECT_TYPE;
	int entityId;
	glm::vec3 color;

	GLObject();
	GLObject(const char*);
	GLObject(GLint);
	~GLObject();

	virtual void render();

	void drawSelectedMarker(bool);

	glm::vec3 getPosition();
	void setPosition(glm::vec3&);
	void setTextureID(GLint);

	static void useShaderProgram(glm::mat4, glm::mat4);
	static void setTileSize(float);
	static void releaseBuffers();
	static GLint Asset(const char*);
protected:
	//FMOD::Sound* moveSound;
	GLint textureID;
	glm::vec3 position;
	bool renderTexture;
	
	static GLuint VBO, VAO, EBO;
	static float tileSize;
	static std::unordered_map<std::string, GLint> assets;
	static GLuint shaderProgram;
};