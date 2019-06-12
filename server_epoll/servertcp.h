#ifndef SERVERTCP_H_
#define SERVERTCP_H_

#include "../common/dataframework.h"
#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <cstring>

#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR -1
#define Handler int

#define RECV_BUFFER_SIZE 1024
#define MAX_CLIENT 1000

typedef enum
{
	SERVER,
	CLIENT
}Hint;

class ClientSocket
{
public:
	ClientSocket(SOCKET socket = INVALID_SOCKET)
			: _socket(socket),
			_lastPos(0)
	{
		memset(_recvMsgBuf, 0, sizeof(_recvMsgBuf));
	}

	int getLastPos()
	{
		return _lastPos;
	}

	char *getMsgBuf()
	{
		return _recvMsgBuf;
	}

	SOCKET getSockfd()
	{
		return _socket;
	}

	void setLastPos(const int pos)
	{
		_lastPos = pos;
	}

private:
	SOCKET _socket;
	char _recvMsgBuf[RECV_BUFFER_SIZE * 10];
	int _lastPos;
};

class ServerTCP
{
public:
	ServerTCP();
	~ServerTCP();

	SOCKET initSocket();
	int bindServer(const std::string ip, const unsigned port);
	int listenServer(const int backlog);
	SOCKET acceptClient();
	void closeSocket();
	bool epollRun();
	bool isRun();
	int recvData(ClientSocket *pClient);
	void onNetMsg(Header *data);
	int sendData(SOCKET _cSock, Header *data);
	void sendData2All(Header *data);

private:
	SOCKET _socket;
	std::vector<ClientSocket*> _clients;
	char _recvBuf[RECV_BUFFER_SIZE];

	Handler _efd;
	bool oneFlag;
	struct epoll_event tep, ep[MAX_CLIENT];

	const std::string getHintInfor(Hint infor);
};

#endif // SERVERTCP_H_
