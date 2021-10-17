#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glad/glad.h> // gl function calls
#include <GLFW/glfw3.h> // openGL calls (GLFW), GLint, GLuint
#include <string> // string
#include <glm/vec3.hpp> // vec3
#include <glm/gtc/matrix_transform.hpp> // glm::mat4, glm::transform, glm::length

#include <GLWrapper/UniformContainer.h>

class Asset
{
public:
	template<typename T>
	static void SetUniform(const std::string&, const T&) {};

	GLint getTextureID();
	void Render(const glm::vec3&,const UniformContainer& uniforms);
	void DrawStatusBar(const glm::vec3& , float);

	int GetNumFrames() {
		return m_numFrames;
	};

private:
	friend class GLObject;
	friend class UniformContainer;

	Asset(const std::string& filename) : m_filename(filename) {};
	~Asset() = default;
	void LoadAsset();
	static void releaseBuffers();

	std::string m_filename;
	GLuint m_textureID;
	int m_width, m_height;
	float m_ratio = 1.0f;
	int m_numFrames = 1;
	bool m_assetLoaded = false;

	static void generateVertexArray();
	static void useShaderProgram(const glm::mat4&, const glm::mat4&, const glm::vec3&);
	static GLuint VBO, VAO, EBO;
	static GLuint shaderProgram;
};