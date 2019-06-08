#ifndef SERVERTCP_H_
#define SERVERTCP_H_

#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include "../common/dataframework.h"

#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR -1

#ifndef RECV_BUFFER_SIZE
	#define RECV_BUFFER_SIZE 1024
#endif // RECV_BUFFER_SIZE

typedef enum
{
	SERVER,
	CLIENT
}Hint;

class ClientSocket //客户端类
{
public:
	ClientSocket(SOCKET sockfd = INVALID_SOCKET)
			: _sockfd(sockfd),
			_lastPos(0)
	{
		memset(_recvMsgBuf, 0, sizeof(_recvMsgBuf));
	}

	SOCKET sockfd() const
	{
		return _sockfd;
	}

	char *msgBuf()
	{
		return _recvMsgBuf;
	}

	int getLastPos() const
	{
		return _lastPos;
	}

	void setLastPos(const int pos)
	{
		_lastPos = pos;
	}

private:
	SOCKET _sockfd;
	char _recvMsgBuf[RECV_BUFFER_SIZE * 10];
	int _lastPos;
};

class ServerTCP
{
public:
	ServerTCP();
	virtual ~ServerTCP();
	
	SOCKET initSocket(); //初始化socket
	int bindHost(const std::string ip, int port); //绑定主机
	int listenHost(const int backlog); //监听主机
	SOCKET acceptClient(); //接受客户端连接
	void closeSocket(); //关闭套接字
	bool isRun(); //检测是否还在运行
	bool selectRun(); //select模型关键函数
	int recvData(ClientSocket *pClient); //接收数据
	virtual void onNetMsg(SOCKET _cSock, Header *data); //处理数据
	int sendData(SOCKET _cSock, Header *data); //发送单个客户端数据
	void sendData2All(Header *data); //给全体客户端发送数据

private:
	SOCKET _socket;
	std::vector<ClientSocket*> _clients;
	char _recvBuf[RECV_BUFFER_SIZE];
	int _recvCount;

	const std::string getHintInfor(Hint infor);
};

#endif // SERVERTCP_H_
