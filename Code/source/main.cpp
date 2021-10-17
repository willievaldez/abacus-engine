#include <Client.h>
#include <Server.h>
#include <thread>

int main()
{
	std::thread client(Client::Init);
	std::thread server(Server::Init);

	client.join();
	server.join();

	return 0;
}
