#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <windows.h>
#include <string>
#include <iostream>
#include <map>
#include <chrono>
#include <glad/glad.h> // gl function calls
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <FMOD/fmod.hpp>
#include <unordered_map>

struct Camera
{
	glm::vec3 pos = glm::vec3(0.0f, 0.0f, 2.0f);		// e  | Position of camera
	glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);	// d  | This is where the camera is looking
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is
	float FOV = 100.0f;

	glm::mat4 GetView() const { return glm::lookAt(pos, pos + direction, up); };
	void SetPos(const glm::vec3& newPos) { pos.x = newPos.x; pos.y = newPos.y; };
};

class Window
{
public:
	static GLFWwindow* InitGLFWWindow();
	static bool InitializeSoundSystem();
	static void InitializeObjects();
	static void ConfigureGLWindow();

	static void IdleCallback3D(GLFWwindow*, clock_t);
	static void IdleCallback2D(GLFWwindow*, clock_t);

	static void DisplayCallback(GLFWwindow*);

	static void ErrorCallback(int error, const char* description);
	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	static const bool* GetKeyMap() { return AccessKeyMap(); };
	static const Camera& GetCamera() { return AccessCamera(); };
	static void SetCameraPos(const glm::vec3& pos) { AccessCamera().SetPos(pos); };

	static void Cleanup();

private:
	static glm::mat4& AccessP() { static glm::mat4 P; return P; };
	static glm::mat4& AccessV() { static glm::mat4 V; return V; };
	static Camera& AccessCamera() { static Camera cam; return cam; };
	static bool* AccessKeyMap() { static bool keyMap[350] = { false }; return keyMap; };
};
