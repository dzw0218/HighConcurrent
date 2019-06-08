#include "servertcp.h"
#include <iostream>

int main()
{
	ServerTCP *server = new ServerTCP();
	server->bindHost("127.0.0.1", 6377);
	server->listenHost(5);
	while(true)
	{
		server->acceptClient();
	}

	return 0;
}
