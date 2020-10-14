#include <GLWrapper/Window.h>
#include <Game/Level.h>
#include <Game/Unit.h>
#include <Config.h>

#include <FMOD/fmod_errors.h>
#include <cmath>

////glm::vec2 mousePos(0.0f);

GLFWwindow* Window::InitGLFWWindow()
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return nullptr;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

	const Config& config = GetConfig();

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(config.windowWidth, config.windowHeight, "Lumaton v0.0.1", NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return nullptr;
	}

	const Camera& cam = GetCamera();
	if (config.frustumType == FrustumType::Perspective)
	{
		AccessP() = glm::perspective(45.0f, (float)config.windowWidth / (float)config.windowHeight, 0.1f, 1000.0f);
	}
	else if (config.frustumType == FrustumType::Orthographic)
	{
		AccessP() = glm::ortho(-config.windowWidth / cam.FOV, config.windowWidth / cam.FOV, -config.windowHeight / cam.FOV, config.windowHeight / cam.FOV, 0.1f, 100.0f);
	}
	AccessV() = cam.GetView();

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// setup callbacks
	glfwSetErrorCallback(ErrorCallback);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	//glfwSetCursorPosCallback(window, cursor_pos_callback);
	//glfwSetFramebufferSizeCallback(window, resize_callback);

	if (config.is3D)
	{
		// disable cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	return window;
}

void Window::ConfigureGLWindow()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (GetConfig().is3D)
	{
		glEnable(GL_DEPTH_TEST);
	}

	glCullFace(GL_BACK);
}

bool Window::InitializeSoundSystem()
{
	FMOD_RESULT result;
	FMOD::System* audioSystem;
	result = FMOD::System_Create(&audioSystem);

	if (result == FMOD_OK)
	{
		result = audioSystem->init(32, FMOD_INIT_NORMAL, nullptr);
	}

	if (result != FMOD_OK)
	{
		std::cout << FMOD_ErrorString(result) << std::endl;
		return false;
	}

	return true;
}

void Window::InitializeObjects()
{
	GLObject::Initialize();

	Level::Get(); // first call to Get will initialize the level
}

//void Window::ResizeCallback(GLFWwindow* window, int width, int height)
//{
//	Window::width = width;
//	Window::height = height;
//
//	// Set the viewport size
//	glViewport(0, 0, width, height);
//
//	if (height > 0)
//	{
//		if (FRUSTUM == FrustumType::Perspective)
//		{
//			P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
//		}
//		else if (FRUSTUM == FrustumType::Orthographic)
//		{
//			P = glm::ortho(-width / FOV, width / FOV, -height / FOV, height / FOV, 0.1f, 100.0f);
//		}
//		V = glm::lookAt(cam_pos, cam_pos + cam_direction, cam_up);
//	}
//}

void Window::IdleCallback3D(GLFWwindow* window, clock_t time)
{
	bool* keyMap = AccessKeyMap();
	Camera& cam = AccessCamera();
	glm::vec3 velocity(0.0f);
	glm::vec3 cam_direction_no_y(cam.direction.x, 0.0f, cam.direction.z);
	if (keyMap[GLFW_KEY_W] || keyMap[GLFW_KEY_UP]) {
		velocity += cam_direction_no_y;
	}
	if (keyMap[GLFW_KEY_A] || keyMap[GLFW_KEY_LEFT]) {
		velocity += glm::cross(cam.up, cam_direction_no_y);
	}
	if (keyMap[GLFW_KEY_S] || keyMap[GLFW_KEY_DOWN]) {
		velocity += cam_direction_no_y * -1.0f;
	}
	if (keyMap[GLFW_KEY_D] || keyMap[GLFW_KEY_RIGHT]) {
		velocity += glm::cross(cam_direction_no_y, cam.up);
	}
	if (keyMap[GLFW_KEY_SPACE])
	{
		velocity.y += 1.0f;
	}
	if (keyMap[GLFW_KEY_LEFT_CONTROL])
	{
		velocity.y -= 1.0f;
	}

	if (!glm::length(velocity) == 0.0f)
	{
		cam.pos += velocity / 100.0f;
	}
}

void Window::IdleCallback2D(GLFWwindow* window, clock_t time)
{
	Level::Get()->Update(time, window);
}

void Window::DisplayCallback(GLFWwindow* window)
{
	if (GetKeyMap()[GLFW_KEY_ESCAPE])
	{
		glfwSetWindowShouldClose(window, true);
		return;
	}

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const Camera& cam = GetCamera();
	AccessV() = cam.GetView();
	GLObject::useShaderProgram(AccessP(), AccessV(), cam.pos);

	Level::Get()->Render();

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();

	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::ErrorCallback(int error, const char* description)
{
	fputs(description, stderr);
}

void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	AccessKeyMap()[key] = !(action == GLFW_RELEASE);
}

void Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	AccessKeyMap()[button] = !(action == GLFW_RELEASE);
}

void Window::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	Camera& cam = AccessCamera();
	if (GetConfig().frustumType == FrustumType::Perspective)
	{
		cam.pos.z -= (float)yoffset;
	}
	else if (GetConfig().frustumType == FrustumType::Orthographic)
	{
		if (yoffset > 0.0f) // zoom in
		{
			if (cam.FOV < 300.0f)
			{
				cam.FOV += 5.0f;
			}
		}
		else // zoom out
		{
			if (cam.FOV > 10.0f)
			{
				cam.FOV -= 5.0f;
			}
		}

		AccessP() = glm::ortho(-GetConfig().windowWidth / cam.FOV, GetConfig().windowWidth / cam.FOV, -GetConfig().windowHeight / cam.FOV, GetConfig().windowHeight / cam.FOV, 0.1f, 100.0f);
	}
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	//if (debug_mode)
	//{
	//	glm::vec4 mouseWorldSpaceVec4((xpos - (Window::width / 2.0)) / (FOV / 2.0f), ((Window::height / 2.0) - ypos) / (FOV / 2.0f), 0.0f, 1.0f);
	//	mouseWorldSpaceVec4.x += cam_pos.x;
	//	mouseWorldSpaceVec4.y += cam_pos.y;
	//	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

	//	if (state == GLFW_PRESS)
	//	{
	//		if (mousePos.x != 0.0f || mousePos.y != 0.0f)
	//		{
	//			float dotProduct = glm::dot(glm::normalize(mousePos), glm::normalize(glm::vec2(mouseWorldSpaceVec4)));
	//			// TODO: calculate difference in size
	//			float rotDelta = glm::degrees(glm::acos(dotProduct));
	//			if (mousePos.x < mouseWorldSpaceVec4.x) rotDelta *= -1.0f;
	//			if (!glm::isnan(rotDelta))
	//			{
	//				rot += rotDelta;
	//			}
	//			std::cout << "rot: " << rot << std::endl;

	//		}

	//		mousePos = mouseWorldSpaceVec4;
	//	}
	//	else if (state == GLFW_RELEASE)
	//	{
	//		mousePos = glm::vec2(0.0f);
	//	}

	//	return;
	//}

	//if (is3D)
	//{
	//	// manipulate the camera
	//	glm::vec2 newMousePos; (xpos, ypos);
	//	newMousePos.x = ((2.0f * (float)xpos) - Window::width) / Window::width;
	//	newMousePos.y = (Window::height - (2.0f * (float)ypos)) / Window::height;

	//	if (glm::length(mousePos) != 0.0f)
	//	{
	//		float sensitivity = 1.0f;
	//		glm::vec2 delta = newMousePos - mousePos;
	//		//printf("delta: (%f, %f)\n", delta.x, delta.y);

	//		glm::vec4 camDirV4(cam_direction, 1.0f);
	//		glm::mat4 xRotationMatrix(1.0f);
	//		xRotationMatrix = glm::rotate(glm::mat4(1.0f), delta.x * sensitivity, glm::vec3(0.0f, -1.0f, 0.0f));
	//		camDirV4 = xRotationMatrix * camDirV4;

	//		glm::mat4 yRotationMatrix(1.0f);
	//		if ((delta.y < 0.0f  && cam_direction.y > -0.95) || (delta.y > 0.0f  && cam_direction.y < 0.95)) 
	//		{
	//			yRotationMatrix = glm::rotate(glm::mat4(1.0f), delta.y * sensitivity, glm::cross(cam_direction,cam_up));
	//			camDirV4 = yRotationMatrix * camDirV4;
	//		}

	//		cam_direction = glm::normalize(glm::vec3(camDirV4));
	//		//printf("cam_direction: (%f, %f, %f)\n", cam_direction.x, cam_direction.y, cam_direction.z);

	//	}

	//	mousePos = newMousePos;

	//}

}

void Window::Cleanup()
{
	GLObject::releaseBuffers();
}