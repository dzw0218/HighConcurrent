#ifndef CLIENTTCP_H_
#define CLIENTTCP_H_

#include "../common/dataframework.h"
#include <iostream>
#include <string>
#include <ctime>
#include <unistd.h>
#include <arpa/inet.h>

#define SOCKET int
#define INVALID_SOCKET (SOCKET)(~0)
#define SOCKET_ERROR -1
#define RECV_BUFFER_SIZE 10240

typedef enum
{
	SERVER,
	CLIENT
}Hint;

class ClientTCP
{
public:
	ClientTCP();
	~ClientTCP();

	int initSocket();
	int connectServer(std::string ip, const unsigned port);
	int closeSocket();
	int recvData(SOCKET _cSock);
	virtual void onNetMsg(Header *data);
	int sendData(Header *data);
	bool selectRun();
	bool isRun() ;

private:
	SOCKET _socket;
	char _recvBuf[RECV_BUFFER_SIZE];
	char _recvMsgBuf[RECV_BUFFER_SIZE * 10];
	int _lastPos;

	const std::string getHintInfor(Hint infor);
};

#endif // CLIENTTCP_H_
