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
float rot = 0.0f;
float disp = 10.0f;

std::vector<GLint> structures;
int whichStructure;
GLObject* pointer;
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
	GLObject::setTileSize(2.0f);

	level = new Level("testLevel.csv");
	glm::vec3 spawn = level->getSpawn();

	cam_look_at = spawn;
	cam_pos = cam_look_at;
	cam_pos.z = 1.0f;

	tileGrid = new GLObject("isogrid.png");

	GLObject::Asset("pixelflag.png");
	GLObject::Asset("datASSet.png");
	GLObject::Asset("demongrunt.png");
	GLObject::Asset("gravestone.png");
	GLObject::Asset("greentarget.png");
	GLObject::Asset("greentarget.png");
	GLObject::Asset("redtarget.png");
	GLObject::Asset("pentagram.png");
	structures.push_back(GLObject::Asset("AstroChurch.png"));
	structures.push_back(GLObject::Asset("turret.png"));
	whichStructure = 0;
	pointer = new GLObject(structures[whichStructure]);
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
	delete pointer;
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
	//glfwSetCursorPosCallback(window, Window::cursor_pos_callback);
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
			cam_pos += velocity / 5.0f;
			cam_look_at += velocity / 5.0f;
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

	level->render(rot);
	tileGrid->render(disp);

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	glm::vec4 mouseWorldSpaceVec4((xpos - (Window::width / 2.0)) / (FOV / 2.0f), ((Window::height / 2.0) - ypos) / (FOV / 2.0f), 0.0f, 1.0f);
	mouseWorldSpaceVec4.x += cam_pos.x;
	mouseWorldSpaceVec4.y += cam_pos.y;

	glm::vec3 mouseWorldSpace = glm::inverse(GLObject::isometricSkew) * mouseWorldSpaceVec4;

	if (MOUSE_MODE == MouseMode::Select)
	{

	}
	else if (MOUSE_MODE == MouseMode::Build)
	{
		std::pair<int, int> tileCoords;
		glm::vec3 tileCenter;
		if (level->getTileFromCoords(mouseWorldSpace, tileCoords) && level->getCoordsFromTile(tileCoords, tileCenter))
		{
			pointer->setPosition(tileCenter);
			pointer->render();
		}

	}

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	key_press[key] = !(action == GLFW_RELEASE);

	//if (action == GLFW_PRESS && key == GLFW_KEY_LEFT)
	//{
	//	printf("displacement: %d\n", disp);
	//	disp--;
	//}
	//if (action == GLFW_PRESS && key == GLFW_KEY_RIGHT)
	//{
	//	printf("displacement: %d\n", disp);
	//	disp++;
	//}

	if (action == GLFW_PRESS && key == GLFW_KEY_R)
	{
		level->reload();
	}
	if (action == GLFW_PRESS && key == GLFW_KEY_B)
	{
		if (MOUSE_MODE == MouseMode::Build)
			MOUSE_MODE = MouseMode::Select;
		else MOUSE_MODE = MouseMode::Build;
	}
	if (MOUSE_MODE == MouseMode::Build)
	{

		if (action == GLFW_PRESS && key == GLFW_KEY_1)
		{
			whichStructure = 0;
			pointer->setTextureID(structures[0]);
		}
		else if (action == GLFW_PRESS && key == GLFW_KEY_2)
		{
			whichStructure = 1;
			pointer->setTextureID(structures[1]);
		}
		//else if (action == GLFW_PRESS && key == GLFW_KEY_3)
		//else if (action == GLFW_PRESS && key == GLFW_KEY_4)

	}
	
}

void Window::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{



}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (key_press[GLFW_KEY_LEFT_CONTROL])
	{
		printf("angle: %f\n",rot);
		rot += yoffset * 0.01f;
	}
	else {
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
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	glm::vec4 mouseWorldSpaceVec4((xpos - (Window::width / 2.0)) / (FOV / 2.0f), ((Window::height / 2.0) - ypos) / (FOV / 2.0f), 0.0f, 1.0f);
	mouseWorldSpaceVec4.x += cam_pos.x;
	mouseWorldSpaceVec4.y += cam_pos.y;

	glm::vec3 mouseWorldSpace = glm::inverse(GLObject::isometricSkew) * mouseWorldSpaceVec4;

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
