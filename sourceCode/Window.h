#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#define INSTALL_DIR std::string("C:\\Users\\Willie\\source\\repos\\Project1\\Project1")

#include <windows.h>
#include <string>
#include <iostream>
#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <FMOD/fmod.hpp>

#include "Shader.h"
#include "GLObject.h"

class Window
{
public:
	static int width;
	static int height;
	static glm::mat4 P;
	static glm::mat4 V;
	static FMOD::System* AudioSystem;
	static void initialize_objects();
	static bool initialize_sound_system();
	static void clean_up();
	static GLFWwindow* create_window(int width, int height);
	static void resize_callback(GLFWwindow* window, int width, int height);
	static void idle_callback();
	static void display_callback(GLFWwindow*);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};
