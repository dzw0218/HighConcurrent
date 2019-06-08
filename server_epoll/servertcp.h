#ifndef SERVERTCP_H_
#define SERVERTCP_H_

#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR -1

#define RECV_BUFFER_SIZE 1024
#define MAX_CLIENT 1000

class ClientSocket
{
public:
	ClientSocket(Socket socket = INVALID_SOCKET)
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
	int BindServer(const std::string ip, const unsigned port);
	int listenServer(const int backlog);
	SOCKET acceptClient();
	void closeSocket();


private:
	SOCKET _socket;
	char _recvBuf[RECV_BUFFER_SIZE];
	struct epoll_event tep, ep[MAX_CLIENT];
};

#endif // SERVERTCP_H_
