#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <vector>
#include <unordered_map>

#define INSTALL_DIR std::string("../../")


class GLObject
{
public:
	int entityId;
	glm::vec3 color;

	GLObject();
	GLObject(const char*);
	~GLObject();

	virtual void render(GLuint&);
	
	glm::vec3 getPosition();
	void setPosition(glm::vec3&);

	GLint getTextureID();

	static void setTileSize(float);
	static void releaseBuffers();
	static GLint addAsset(const char*);
protected:
	//FMOD::Sound* moveSound;
	GLint textureID;

	bool renderTexture;
	
	static GLuint VBO, VAO, EBO;
	static float tileSize;
	glm::vec3 position;
	static std::unordered_map<std::string, GLint> assets;

};