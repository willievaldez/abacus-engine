#include <Server.h>

// 3rd party libraries
#include <enet/enet.h>

// Abacus libraries
#include <Game/Level.h>
#include <Utility/Config.h>
#include <Utility/KeyMap.h>

#include <unordered_map>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

const char* const Server::ProcessName = "Server";

void Server::Init()
{
	const Config& config = GetConfig(ProcessName);
	if (config.initServer)
	{
		// initialize ENet networking library
		if (enet_initialize() != 0)
		{
			fprintf(stderr, "An error occurred while initializing ENet.\n");
			exit(EXIT_FAILURE);
		}

		atexit(enet_deinitialize);

		ENetAddress address;
		ENetHost* server;
		std::unordered_map<ENetPeer*, KeyMap> clients;
		/* Bind the server to the default localhost.     */
		/* A specific host address can be specified by   */
		/* enet_address_set_host (& address, "x.x.x.x"); */
		address.host = ENET_HOST_ANY;
		/* Bind the server to port 1234. */
		address.port = 1234;
		server = enet_host_create(&address /* the address to bind the server host to */,
			32      /* allow up to 32 clients and/or outgoing connections */,
			2      /* allow up to 2 channels to be used, 0 and 1 */,
			0      /* assume any amount of incoming bandwidth */,
			0      /* assume any amount of outgoing bandwidth */);
		if (server == NULL)
		{
			fprintf(stderr,
				"An error occurred while trying to create an ENet server host.\n");
			exit(EXIT_FAILURE);
		}

		printf("Server hosted on port %d\n", address.port);

		Level* level = Level::Get();

		float secondsPerTick = 1.0f / config.ticksPerSecond;
		clock_t lastIdleCallback = clock();

		while (true)
		{
			clock_t now = clock();
			float secondsSinceLastCallback = (now - lastIdleCallback) / (float)CLOCKS_PER_SEC;
			if (secondsSinceLastCallback >= secondsPerTick)
			{
				if (clients.size() != 0)
				{
					level->Update(clients.begin()->second);
					lastIdleCallback = now;
				}

				for (auto client : clients)
				{
					/* Create a reliable packet of size 7 containing "packet\0" */
					ENetPacket* packet = enet_packet_create("packet",
						strlen("packet") + 1,
						ENET_PACKET_FLAG_RELIABLE);
					/* Extend the packet so and append the string "foo", so it now */
					/* contains "packetfoo\0"                                      */
					//enet_packet_resize(packet, strlen("packetfoo") + 1);
					//strcpy(&packet->data[strlen("packet")], "foo");
					/* Send the packet to the peer over channel id 0. */
					/* One could also broadcast the packet by         */
					/* enet_host_broadcast (host, 0, packet);         */
					enet_peer_send(client.first, 0, packet);

					/* One could just use enet_host_service() instead. */
					enet_host_flush(server);
				}
			}

			// poll for net events
			ENetEvent netEvent;
			while (enet_host_service(server, &netEvent, 0) > 0)
			{
				//printf("===========NEW EVENT===========\n");
				switch (netEvent.type)
				{
				case ENET_EVENT_TYPE_CONNECT:
					printf("SERVER: A new client connected from %x:%u.\n",
						netEvent.peer->address.host,
						netEvent.peer->address.port);
					/* Store any relevant client information here. */
					netEvent.peer->data = (void*)"ClientInfo";
					break;
				case ENET_EVENT_TYPE_RECEIVE:
				{
					//printf("SERVER: A packet of length %zu was received from %s on channel %u.\n",
					//	netEvent.packet->dataLength,
					//	(char*)netEvent.peer->data,
					//	netEvent.channelID);
					int keyMapPacket[KeyMap::GetPacketArraySize()];
					memcpy(keyMapPacket, netEvent.packet->data, netEvent.packet->dataLength);
					clients[netEvent.peer] = KeyMap(keyMapPacket);

					/* Clean up the packet now that we're done using it. */
					enet_packet_destroy(netEvent.packet);

					break;
				}
				case ENET_EVENT_TYPE_DISCONNECT:
					printf("SERVER: %s disconnected.\n", (char*)netEvent.peer->data);
					/* Reset the peer's client information. */
					netEvent.peer->data = NULL;
				}
			}
		}

		printf("shutting down server...\n");

		enet_host_destroy(server);
	}
}
