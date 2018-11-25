#include "Window.h"
#include "Level.h"
#include "Unit.h"

#include <FMOD/fmod_errors.h>
#include <cmath>

enum MovementType {
	Keyboard,
	PointAndClick,
	Controller
};

enum FrustumType {
	Perspective,
	Orthographic
};

enum MouseMode {
	Select,
	Build
};

const char* window_title = "Occultech v0.0.1";
MovementType MOVEMENT = PointAndClick;
FrustumType FRUSTUM = Orthographic;
MouseMode MOUSE_MODE = Select;

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 1.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is
std::map <int, bool> key_press;

glm::vec2 mousePos(0.0f);
float ySize = 1.0f;
float rot = -13.0f;
bool debug_mode = true;

std::vector<Asset*> structures;
int whichStructure;
Asset* pointer;
Level* level;

int Window::width = 1280;
int Window::height = 720;
float FOV = 100.0f;

glm::mat4 Window::P;
glm::mat4 Window::V;

FMOD::System* Window::AudioSystem;

GLObject* tileGrid;

void Window::initialize_objects()
{
	GLObject::initialize();

	level = new Level("testLevel.csv");
	glm::vec3 spawn = level->getSpawn();

	cam_look_at = spawn;
	cam_pos = cam_look_at;
	cam_pos.z = 1.0f;

	tileGrid = new GLObject("isogrid.png");

	GLObject::GLAsset("pixelflag.png");
	GLObject::GLAsset("datASSet.png");
	GLObject::GLAsset("demongrunt.png");
	GLObject::GLAsset("gravestone.png");
	GLObject::GLAsset("greentarget.png");
	GLObject::GLAsset("greentarget.png");
	GLObject::GLAsset("redtarget.png");
	GLObject::GLAsset("pentagram.png");
	structures.push_back(GLObject::GLAsset("AstroChurch.png"));
	structures.push_back(GLObject::GLAsset("turret.png"));
	whichStructure = 0;
	pointer = structures[0];
}

bool Window::initialize_sound_system()
{
	FMOD_RESULT result;
	result = FMOD::System_Create(&Window::AudioSystem);

	if (result == FMOD_OK)
	{
		result = AudioSystem->init(32, FMOD_INIT_NORMAL, nullptr);
	}

	if (result != FMOD_OK)
	{
		std::cout << FMOD_ErrorString(result) << std::endl;
		return false;
	}


	return true;
}

void Window::clean_up()
{
	delete level;
	delete tileGrid;
	GLObject::releaseBuffers();
}


void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

void setup_callbacks(GLFWwindow* window)
{
	glfwSetErrorCallback(error_callback);
	glfwSetKeyCallback(window, Window::key_callback);
	glfwSetScrollCallback(window, Window::scroll_callback);
	glfwSetCursorPosCallback(window, Window::cursor_pos_callback);
	glfwSetFramebufferSizeCallback(window, Window::resize_callback);
	glfwSetMouseButtonCallback(window, Window::mouse_button_callback);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	if (FRUSTUM == FrustumType::Perspective)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
	}
	else if (FRUSTUM == FrustumType::Orthographic)
	{
		P = glm::ortho(-width / FOV, width / FOV, -height / FOV, height / FOV, 0.1f, 100.0f);
	}
	V = glm::lookAt(cam_pos, cam_look_at, cam_up);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	setup_callbacks(window);

	return window;
}

void Window::configure_gl_window()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glCullFace(GL_BACK);
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
	Window::width = width;
	Window::height = height;

	// Set the viewport size
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		if (FRUSTUM == FrustumType::Perspective)
		{
			P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		}
		else if (FRUSTUM == FrustumType::Orthographic)
		{
			P = glm::ortho(-width / FOV, width / FOV, -height / FOV, height / FOV, 0.1f, 100.0f);
		}
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback(clock_t time)
{
	if (MOVEMENT == MovementType::Keyboard)
	{
		glm::vec3 velocity(0.0f);
		if (key_press[GLFW_KEY_W] || key_press[GLFW_KEY_UP]) {
			velocity.y += 1.0f;
		}
		if (key_press[GLFW_KEY_A] || key_press[GLFW_KEY_LEFT]) {
			velocity.x -= 1.0f;
		}
		if (key_press[GLFW_KEY_S] || key_press[GLFW_KEY_DOWN]) {
			velocity.y -= 1.0f;
		}
		if (key_press[GLFW_KEY_D] || key_press[GLFW_KEY_RIGHT]) {
			velocity.x += 1.0f;
		}

		if (!glm::length(velocity) == 0.0f)
		{
			// put player movement method call here
		}
	}
	else if (MOVEMENT == MovementType::PointAndClick)
	{
		glm::vec3 velocity(0.0f);
		if (key_press[GLFW_KEY_W] || key_press[GLFW_KEY_UP]) {
			velocity.y += 1.0f;
		}
		if (key_press[GLFW_KEY_A] || key_press[GLFW_KEY_LEFT]) {
			velocity.x -= 1.0f;
		}
		if (key_press[GLFW_KEY_S] || key_press[GLFW_KEY_DOWN]) {
			velocity.y -= 1.0f;
		}
		if (key_press[GLFW_KEY_D] || key_press[GLFW_KEY_RIGHT]) {
			velocity.x += 1.0f;
		}

		if (!glm::length(velocity) == 0.0f) {
			cam_pos += velocity / 3.0f;
			cam_look_at += velocity / 3.0f;
		}
	}

	level->update(time);
}

void Window::display_callback(GLFWwindow* window)
{
	if (key_press[GLFW_KEY_ESCAPE])
	{
		glfwSetWindowShouldClose(window, true);
		return;
	}

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	GLObject::useShaderProgram(P, V);

	level->render(0.13f, rot);

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	glm::vec4 mouseWorldSpaceVec4((xpos - (Window::width / 2.0)) / (FOV / 2.0f), ((Window::height / 2.0) - ypos) / (FOV / 2.0f), 0.0f, 1.0f);
	mouseWorldSpaceVec4.x += cam_pos.x;
	mouseWorldSpaceVec4.y += cam_pos.y;

	glm::vec3 mouseWorldSpace = glm::inverse(GLObject::getIsometricSkew()) * mouseWorldSpaceVec4;

	if (MOUSE_MODE == MouseMode::Select)
	{

	}
	else if (MOUSE_MODE == MouseMode::Build)
	{
		std::pair<int, int> tileCoords;
		glm::vec3 tileCenter;
		if (level->getTileFromCoords(mouseWorldSpace, tileCoords) && level->getCoordsFromTile(tileCoords, tileCenter))
		{
			pointer->render(tileCenter);
		}

	}

	GLObject::drawDebugLine(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 30.0f, 0.0f));

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	key_press[key] = !(action == GLFW_RELEASE);

	


	if (action == GLFW_PRESS && key == GLFW_KEY_R)
	{
		level->reload();
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_B)
	{
		if (MOUSE_MODE == MouseMode::Build)
			MOUSE_MODE = MouseMode::Select;
		else MOUSE_MODE = MouseMode::Build;
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_BACKSLASH)
	{
		debug_mode = !debug_mode;
	}
	if (MOUSE_MODE == MouseMode::Build)
	{

		if (action == GLFW_PRESS && key == GLFW_KEY_1)
		{
			whichStructure = 0;
			pointer = structures[0];
		}
		else if (action == GLFW_PRESS && key == GLFW_KEY_2)
		{
			whichStructure = 1;
			pointer = structures[1];
		}
		//else if (action == GLFW_PRESS && key == GLFW_KEY_3)
		//else if (action == GLFW_PRESS && key == GLFW_KEY_4)

	}
	
}

void Window::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (debug_mode)
	{
		glm::vec4 mouseWorldSpaceVec4((xpos - (Window::width / 2.0)) / (FOV / 2.0f), ((Window::height / 2.0) - ypos) / (FOV / 2.0f), 0.0f, 1.0f);
		mouseWorldSpaceVec4.x += cam_pos.x;
		mouseWorldSpaceVec4.y += cam_pos.y;
		int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

		if (state == GLFW_PRESS)
		{
			if (mousePos.x != 0.0f || mousePos.y != 0.0f)
			{
				float dotProduct = glm::dot(glm::normalize(mousePos), glm::normalize(glm::vec2(mouseWorldSpaceVec4)));
				// TODO: calculate difference in size
				float rotDelta = glm::degrees(glm::acos(dotProduct));
				if (mousePos.x < mouseWorldSpaceVec4.x) rotDelta *= -1.0f;
				if (!glm::isnan(rotDelta))
				{
					rot += rotDelta;
				}
				std::cout << "rot: " << rot << std::endl;

			}

			mousePos = mouseWorldSpaceVec4;
		}
		else if (state == GLFW_RELEASE)
		{
			mousePos = glm::vec2(0.0f);
		}

		return;
	}


}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

	if (FRUSTUM == FrustumType::Perspective)
	{
		cam_pos.z += yoffset;
		cam_look_at.z += yoffset;
	}
	else if (FRUSTUM == FrustumType::Orthographic)
	{
		if (yoffset > 0.0f) // zoom in
		{
			if (FOV < 200.0f)
			{
				FOV += 5.0f;
			}
		}
		else // zoom out
		{
			if (FOV > 10.0f)
			{
				FOV -= 5.0f;
			}
		}

		P = glm::ortho(-width / FOV, width / FOV, -height / FOV, height / FOV, 0.1f, 100.0f);
	}
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	glm::vec4 mouseWorldSpaceVec4((xpos - (Window::width / 2.0)) / (FOV / 2.0f), ((Window::height / 2.0) - ypos) / (FOV / 2.0f), 0.0f, 1.0f);
	mouseWorldSpaceVec4.x += cam_pos.x;
	mouseWorldSpaceVec4.y += cam_pos.y;

	if (debug_mode)
	{
		if (action == GLFW_PRESS)
		{
			
			if (mousePos.x != 0.0f || mousePos.y != 0.0f)
			{
				// TODO: calculate difference in size
				float rotDelta = glm::acos(glm::dot(glm::normalize(mousePos), glm::normalize(glm::vec2(mouseWorldSpaceVec4))));
				rot += rotDelta;
				std::cout << "delta: " << rotDelta << std::endl;

			}

			mousePos = mouseWorldSpaceVec4;
		}
		else if (action == GLFW_RELEASE)
		{
			mousePos = glm::vec2(0.0f);
		}
	}
	else 
	{
		glm::vec3 mouseWorldSpace = glm::inverse(GLObject::getIsometricSkew()) * mouseWorldSpaceVec4;

		if (action == GLFW_PRESS)
		{
			if (MOUSE_MODE == MouseMode::Select)
			{

				if (button == GLFW_MOUSE_BUTTON_1)
				{
					level->selectUnit(mouseWorldSpace, key_press[GLFW_KEY_LEFT_SHIFT]);
				}
				else if (button == GLFW_MOUSE_BUTTON_2)
				{
					level->addTarget(mouseWorldSpace, key_press[GLFW_KEY_LEFT_SHIFT]);
				}
			}
			else if (MOUSE_MODE == MouseMode::Build)
			{
				level->placeStructure(mouseWorldSpace, structures[whichStructure]);

				if (!key_press[GLFW_KEY_LEFT_SHIFT])
				{
					MOUSE_MODE = MouseMode::Select;
				}
			}
		}
		else if (action == GLFW_RELEASE)
		{

		}
	}

	
}
