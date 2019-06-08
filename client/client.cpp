#include "clienttcp.h"
#include "../common/dataframework.h"
#include <iostream>

int main()
{
	ClientTCP *client = new ClientTCP();
	client->connectServer("127.0.0.1", 6377);

	while(true)
	{
		
	}

	return 0;
}
