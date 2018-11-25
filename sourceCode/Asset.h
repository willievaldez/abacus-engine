#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h> // gl function calls
#include <GLFW/glfw3.h> // openGL calls (GLFW), GLint, GLuint
#include <string> // string
#include <glm/vec3.hpp> // vec3
#include <glm/gtc/matrix_transform.hpp> // glm::mat4, glm::transform, glm::length


class Asset
{
public:
	GLint getTextureID();
	void render(glm::vec3&);
private:
	friend class GLObject;

	Asset(std::string&);
	~Asset();
	static void releaseBuffers();

	GLuint textureID;
	int width, height;
	float ratio;

	static void draw_debug_line(glm::vec3, glm::vec3);
	static void setIsometricSkew(float skew, float rotation);
	static void generateVertexArray();
	static void useShaderProgram(glm::mat4&, glm::mat4&);
	static glm::mat4 isometricSkew;
	static GLuint VBO, VAO, EBO;
	static GLuint shaderProgram;

};