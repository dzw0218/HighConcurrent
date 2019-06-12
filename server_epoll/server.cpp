#include "servertcp.h"
#include <iostream>

int main()
{
	ServerTCP *server = new ServerTCP();
	server->bindServer("127.0.0.1", 6378);
	server->listenServer(5);

	while(true)
	{
		server->acceptClient();
	}
	return 0;
}
