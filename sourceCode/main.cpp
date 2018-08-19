#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <steam/steam_api.h> // I changed some function calls to "safe function calls" in matchmaking

#include "stdafx.h"
#include "Window.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

bool useSteam = false;

// settings
GLFWwindow* window;

int main()
{

	if (useSteam)
	{
		if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid))
		{
			// if Steam is not running or the game wasn't started through Steam, SteamAPI_RestartAppIfNecessary starts the 
			// local Steam client and also launches this game again.

			// Once you get a public Steam AppID assigned for this game, you need to replace k_uAppIdInvalid with it and
			// removed steam_appid.txt from the game depot.

			return EXIT_FAILURE;
		}

		// Init Steam CEG
		if (!Steamworks_InitCEGLibrary())
		{
			OutputDebugString("Steamworks_InitCEGLibrary() failed\n");
			//Alert("Fatal Error", "Steam must be running to play this game (InitDrmLibrary() failed).\n");
			return EXIT_FAILURE;
		}

		// Initialize SteamAPI, if this fails we bail out since we depend on Steam for lots of stuff.
		// You don't necessarily have to though if you write your code to check whether all the Steam
		// interfaces are NULL before using them and provide alternate paths when they are unavailable.
		//
		// This will also load the in-game steam overlay dll into your process.  That dll is normally
		// injected by steam when it launches games, but by calling this you cause it to always load,
		// even when not launched via steam.
		if (!SteamAPI_Init())
		{
			OutputDebugString("SteamAPI_Init() failed\n");
			//Alert("Fatal Error", "Steam must be running to play this game (SteamAPI_Init() failed).\n");
			return EXIT_FAILURE;
		}

		// Ensure that the user has logged into Steam. This will always return true if the game is launched
		// from Steam, but if Steam is at the login prompt when you run your game from the debugger, it
		// will return false.
		if (!SteamUser()->BLoggedOn())
		{
			OutputDebugString("Steam user is not logged in\n");
			//Alert("Fatal Error", "Steam user must be logged in to play this game (SteamUser()->BLoggedOn() returned false).\n");
			return EXIT_FAILURE;
		}

		// do a DRM self check
		Steamworks_SelfCheck();

	}
	

	window = Window::create_window(Window::width, Window::height);
	if (window == NULL) return EXIT_FAILURE;


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return EXIT_FAILURE;
	}

	Window::configure_gl_window();

	if (!Window::initialize_sound_system()) return EXIT_FAILURE;


	Window::initialize_objects();


	std::thread([]() {
		while (!glfwWindowShouldClose(window))
		{
			clock_t begin = clock();
			Window::idle_callback(begin);
			clock_t end = clock();

			float milisecondsElapsed = 1000.0f * (end - begin) / CLOCKS_PER_SEC;

			if ((1000/60) - (milisecondsElapsed) < 0) {
				std::cout << "WARNING: TICK RATE IS TOO SLOW" << std::endl;
			}
			else {
				std::this_thread::sleep_for(std::chrono::milliseconds((int)((1000 / 60) - (milisecondsElapsed))));
			}
		}
	}).detach();


	while (!glfwWindowShouldClose(window))
	{
		Window::display_callback(window);
	}


	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();

	if (useSteam)
	{
		// Shutdown the SteamAPI
		SteamAPI_Shutdown();

		// Shutdown Steam CEG
		Steamworks_TermCEGLibrary();
	}

	return EXIT_SUCCESS;
}
