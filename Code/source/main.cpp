#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <steam/steam_api.h> // I changed some function calls to "safe function calls" in matchmaking
#include <openVR/openvr.h>

#include <stdafx.h>
#include <GLWrapper/Window.h>
#include <Utility/Config.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

// Why is this here? because https://gamedev.stackexchange.com/questions/158106/why-am-i-getting-these-errors-when-including-stb-image-h
#define STB_IMAGE_IMPLEMENTATION
#include <std_image.h> // stbi_load

int main()
{
	const Config& config = GetConfig();
	if (config.useSteam)
	{
		if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid))
		{
			// if Steam is not running or the game wasn't started through Steam, SteamAPI_RestartAppIfNecessary starts the 
			// local Steam client and also launches this game again.

			// Once you get a public Steam AppID assigned for this game, you need to replace k_uAppIdInvalid with it and
			// removed steam_appid.txt from the game depot.
			OutputDebugString("Invalid App ID\n");
			return EXIT_FAILURE;
		}

		// Init Steam CEG
		if (!Steamworks_InitCEGLibrary())
		{
			OutputDebugString("Steamworks_InitCEGLibrary() failed\n");
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
	
	vr::IVRSystem* vr_pointer = nullptr;
	if (config.useVR)
	{
		vr::EVRInitError eError = vr::VRInitError_None;
		vr_pointer = vr::VR_Init(&eError, vr::VRApplication_Background);
		if (eError != vr::VRInitError_None)
		{
			vr_pointer = nullptr;
			printf("Unable to init VR runtime: %s \n",
				VR_GetVRInitErrorAsEnglishDescription(eError));
			exit(EXIT_FAILURE);
		}
	}

	GLFWwindow* window = Window::InitGLFWWindow();
	if (window == nullptr) {
		OutputDebugString("Failed to create window\n");
		return EXIT_FAILURE;
	}


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		OutputDebugString("Failed to initialize GLAD\n");
		return EXIT_FAILURE;
	}

	Window::ConfigureGLWindow();

	if (!Window::InitializeSoundSystem()) {
		OutputDebugString("Failed to initialize sound system\n");
		return EXIT_FAILURE;
	}

	Window::InitializeObjects();

	auto idleCallback = Window::IdleCallback2D;
	if (config.is3D)
	{
		idleCallback = Window::IdleCallback3D;
	}

	float secondsPerTick = 1.0f / config.ticksPerSecond;
	clock_t lastIdleCallback = clock();
	while (!glfwWindowShouldClose(window))
	{
		clock_t begin = clock();
		float secondsSinceLastCallback = (begin - lastIdleCallback) / (float)CLOCKS_PER_SEC;
		if (secondsSinceLastCallback >= secondsPerTick)
		{
			lastIdleCallback = begin;
			idleCallback(window, begin);
		}
		Window::DisplayCallback(window);
	}

	Window::Cleanup();

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();

	if (config.useSteam)
	{
		// Shutdown the SteamAPI
		SteamAPI_Shutdown();

		// Shutdown Steam CEG
		Steamworks_TermCEGLibrary();
	}

	if (vr_pointer != nullptr)
	{
		vr::VR_Shutdown();
		vr_pointer = nullptr;
	}

	return EXIT_SUCCESS;
}
