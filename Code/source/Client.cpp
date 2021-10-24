#include <Client.h>

// std libraries
#include <iostream>
#include <stdafx.h>

// 3rd party libraries
#include <enet/enet.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <openVR/openvr.h>
#include <steam/steam_api.h> // I changed some function calls to "safe function calls" in matchmaking

// Why is this here? because https://gamedev.stackexchange.com/questions/158106/why-am-i-getting-these-errors-when-including-stb-image-h
#define STB_IMAGE_IMPLEMENTATION
#include <std_image.h> // stbi_load

// Abacus libraries
#include <GLWrapper/Window.h>
#include <Utility/Config.h>


#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

const char* const Client::ProcessName = "Client";

void Client::Init()
{
	Config config = GetConfig(ProcessName);
	if (config.initClient)
	{
		// initialize ENet networking library
		if (enet_initialize() != 0)
		{
			fprintf(stderr, "An error occurred while initializing ENet.\n");
			exit(EXIT_FAILURE);
		}

		atexit(enet_deinitialize);

		if (config.useSteam)
		{
			if (SteamAPI_RestartAppIfNecessary(k_uAppIdInvalid))
			{
				// if Steam is not running or the game wasn't started through Steam, SteamAPI_RestartAppIfNecessary starts the 
				// local Steam client and also launches this game again.

				// Once you get a public Steam AppID assigned for this game, you need to replace k_uAppIdInvalid with it and
				// removed steam_appid.txt from the game depot.
				printf("Invalid App ID\n");
				exit(EXIT_FAILURE);
			}

			// Init Steam CEG
			if (!Steamworks_InitCEGLibrary())
			{
				printf("Steamworks_InitCEGLibrary() failed\n");
				exit(EXIT_FAILURE);
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
				printf("SteamAPI_Init() failed\n");
				//Alert("Fatal Error", "Steam must be running to play this game (SteamAPI_Init() failed).\n");
				exit(EXIT_FAILURE);
			}

			// Ensure that the user has logged into Steam. This will always return true if the game is launched
			// from Steam, but if Steam is at the login prompt when you run your game from the debugger, it
			// will return false.
			if (!SteamUser()->BLoggedOn())
			{
				printf("Steam user is not logged in\n");
				//Alert("Fatal Error", "Steam user must be logged in to play this game (SteamUser()->BLoggedOn() returned false).\n");
				exit(EXIT_FAILURE);
			}

			// do a DRM self check
			Steamworks_SelfCheck();

			printf("Connection to steam successful!\n");
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

			printf("VR initialized!\n");
		}

		GLFWwindow* window = Window::GetGLFWWindow();
		if (!window) {
			printf("Failed to create GLFW window\n");
			exit(EXIT_FAILURE);
		}

		printf("GLFW Window created!\n");

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			printf("Failed to initialize GLAD\n");
			exit(EXIT_FAILURE);
		}

		printf("GLAD loaded!\n");

		Window::ConfigureGLWindow();

		if (!Window::InitializeSoundSystem()) {
			OutputDebugString("Failed to initialize sound system\n");
			exit(EXIT_FAILURE);
		}

		printf("Sound System initialized!\n");

		// initialize connection to server and get level objects
		ENetHost* client;
		client = enet_host_create(NULL /* create a client host */,
			1 /* only allow 1 outgoing connection */,
			2 /* allow up 2 channels to be used, 0 and 1 */,
			0 /* assume any amount of incoming bandwidth */,
			0 /* assume any amount of outgoing bandwidth */);
		if (client == NULL)
		{
			fprintf(stderr,
				"An error occurred while trying to create an ENet client host.\n");
			exit(EXIT_FAILURE);
		}

		printf("ENet host initialized!\n");

		static const char* const host = "localhost";
		ENetAddress address;
		ENetEvent netEvent;
		ENetPeer* peer;
		/* Connect to some.server.net:1234. */
		enet_address_set_host(&address, host);
		address.port = 1234;
		/* Initiate the connection, allocating the two channels 0 and 1. */
		peer = enet_host_connect(client, &address, 2, 0);
		if (peer == NULL)
		{
			fprintf(stderr,
				"No available peers for initiating an ENet connection.\n");
			exit(EXIT_FAILURE);
		}
		/* Wait up to 5 seconds for the connection attempt to succeed. */
		if (enet_host_service(client, &netEvent, 5000) > 0 &&
			netEvent.type == ENET_EVENT_TYPE_CONNECT)
		{
			printf("CLIENT: Connection to %s:%d succeeded.\n", host, address.port);
		}
		else
		{
			/* Either the 5 seconds are up or a disconnect event was */
			/* received. Reset the peer in the event the 5 seconds   */
			/* had run out without any significant event.            */
			enet_peer_reset(peer);
			printf("CLIENT: Connection to %s:%d failed./n", host, address.port);
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
			clock_t now = clock();
			float secondsSinceLastCallback = (now - lastIdleCallback) / (float)CLOCKS_PER_SEC;
			if (secondsSinceLastCallback >= secondsPerTick)
			{
				lastIdleCallback = now;
				idleCallback(window, now);

				/* Check for an event. */
				while (enet_host_service(client, &netEvent, 0) > 0)
				{
					switch (netEvent.type)
					{
					case ENET_EVENT_TYPE_CONNECT:
						printf("CLIENT: A new client connected from %x:%u.\n",
							netEvent.peer->address.host,
							netEvent.peer->address.port);
						/* Store any relevant client information here. */
						netEvent.peer->data = (void*)"ClientInfo";
						break;
					case ENET_EVENT_TYPE_RECEIVE:
						//printf("CLIENT: A packet of length %zu containing %s was received from %s on channel %u.\n",
						//	netEvent.packet->dataLength,
						//	netEvent.packet->data,
						//	(char*)netEvent.peer->data,
						//	netEvent.channelID);
						/* Clean up the packet now that we're done using it. */
						enet_packet_destroy(netEvent.packet);
						break;
					case ENET_EVENT_TYPE_DISCONNECT:
						printf("CLIENT: %s disconnected.\n", (char*)netEvent.peer->data);
						/* Reset the peer's client information. */
						netEvent.peer->data = NULL;
						break;
					}
				}

				const int* keyMapPacket = Window::GetKeyMap().ToPacket();				
				ENetPacket* packet = enet_packet_create(keyMapPacket,
					KeyMap::GetPacketArraySize() * sizeof(int),
					ENET_PACKET_FLAG_RELIABLE);
				enet_peer_send(peer, 0, packet);
				enet_host_flush(client);

				Window::DisplayCallback(window);
			}
		}

		Window::Cleanup();

		enet_host_destroy(client);

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
	}
}
