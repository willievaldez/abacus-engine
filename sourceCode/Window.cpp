#include "Window.h"
#include <FMOD/fmod_errors.h>

using namespace std;

const char* window_title = "ERASv0.0.1";


GLint shaderProgram;

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 20.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is
std::map <int, bool> key_press;

glm::vec2 center((float)Window::width / 2.0f, (float)Window::height / 2.0f);

GLObject* triangle;

int Window::width = 800;
int Window::height = 600;

glm::mat4 Window::P;
glm::mat4 Window::V;

FMOD::System* Window::AudioSystem;


void Window::initialize_objects()
{
	//triangle = new GLObject((INSTALL_DIR + "/Models/Space_Character.obj").c_str());
	std::vector<GLuint> indices = { 0,1,2 };
	std::vector<glm::vec3> vertices = {
		glm::vec3(-0.5f, -0.5f, 0.0f),
		glm::vec3(0.5f, -0.5f, 0.0f),
		glm::vec3(0.0f,  0.5f, 0.0f)
	};
	std::vector<glm::vec3> normals = {
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)
	};

	triangle = new GLObject(indices, vertices, normals);
	shaderProgram = LoadShaders((INSTALL_DIR+"sourceCode/shader.vert").c_str(), (INSTALL_DIR+"sourceCode/shader.frag").c_str());
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
		cout << FMOD_ErrorString(result) << endl;
		return false;
	}


	return true;
}

void Window::clean_up()
{
	glDeleteProgram(shaderProgram);
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
	//glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, 1);
	glfwSetCursorPosCallback(window, Window::cursor_pos_callback);
	glfwSetFramebufferSizeCallback(window, Window::resize_callback);
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

	P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
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

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
	Window::width = width;
	Window::height = height;

	center = glm::vec2((float)Window::width / 2.0f, (float)Window::height / 2.0f);

	// Set the viewport size
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
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
		triangle->move(glm::normalize(velocity)/10.0f);
}

void Window::display_callback(GLFWwindow* window)
{
	if (key_press[GLFW_KEY_ESCAPE])
	{
		glfwSetWindowShouldClose(window, true);
		return;
	}

	glClearColor(0.0f, 0.1f, 0.1f, 1.0f);
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDepthMask(GL_TRUE);

	// Use the shader of programID
	glUseProgram(shaderProgram);
	V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	GLuint MatrixID = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &Window::V[0][0]);
	MatrixID = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &Window::P[0][0]);
	MatrixID = glGetUniformLocation(shaderProgram, "cameraPos");
	glUniform3f(MatrixID, (cam_pos.x), (cam_pos.y), (cam_pos.z));

	triangle->render(shaderProgram);

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	key_press[key] = !(action == GLFW_RELEASE);
}

void Window::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{

}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	triangle->move(glm::vec3(0.0f, 0.0f, (float)yoffset));
}
