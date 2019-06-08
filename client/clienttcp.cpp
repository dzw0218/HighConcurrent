#include "clienttcp.h"
#include <cstring>

ClientTCP::ClientTCP()
		: _lastPos(0),
		_socket(INVALID_SOCKET)
{
	memset(_recvBuf, 0, sizeof(_recvBuf));
	memset(_recvMsgBuf, 0, sizeof(_recvMsgBuf));
}

ClientTCP::~ClientTCP()
{
	closeSocket();
}

int ClientTCP::initSocket()
{
	if(_socket != INVALID_SOCKET)
	{
		std::cout << getHintInfor(CLIENT) << "has a old connection." << std::endl;
		return -1;
	}
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(SOCKET_ERROR == _socket)
	{
		std::cout << getHintInfor(CLIENT) << "initialized unsuccessfully." << std::endl;
		return -1;
	}
	else
		std::cout << getHintInfor(CLIENT) << "initialized successfully." << std::endl;
	return 0;
}

int ClientTCP::connectServer(std::string ip, const unsigned port)
{
	if(INVALID_SOCKET == _socket)
		initSocket();
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(port);
	_sin.sin_addr.s_addr = inet_addr(ip.c_str());
	int ret = connect(_socket, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if(SOCKET_ERROR == ret)
	{
		std::cout << getHintInfor(CLIENT) << "connected to server failed." << std::endl;
		return -1;
	}
	else
		std::cout << getHintInfor(CLIENT) << "connect to " << ip << ":" << port << " successfully." << std::endl;
	return ret;
}

int ClientTCP::closeSocket()
{
	if(_socket != INVALID_SOCKET)
	{
		close(_socket);
	}
	_socket = INVALID_SOCKET;
}

int ClientTCP::recvData(SOCKET _cSock)
{
	int recvLen = recv(_cSock, _recvBuf, sizeof(_recvBuf), 0);
	if(recvLen <= 0)
	{
		std::cout << getHintInfor(CLIENT) << "server connection broken." << std::endl;
		return SOCKET_ERROR;
	}

	memcpy(_recvMsgBuf + _lastPos, _recvBuf, recvLen);
	_lastPos += recvLen;
	while(_lastPos >= sizeof(Header))
	{
		Header *data = (Header*)_recvMsgBuf;
		if(_lastPos >= data->length)
		{
			int _nSize = _lastPos - data->length;
			//处理数据
			memcpy(_recvMsgBuf, _recvMsgBuf + data->length, _nSize);
			_lastPos = _nSize;
		}
		else
			break;
	}
	return 1;
}

void ClientTCP::onNetMsg(Header *data)
{
	
}

int ClientTCP::sendData(Header *data)
{
	if(isRun() && data)
		return send(_socket, (const char*)data, data->length, 0);
	return SOCKET_ERROR;
}

bool ClientTCP::selectRun()
{
	if(isRun())
	{
		fd_set fdRead;
		FD_ZERO(&fdRead);
		FD_SET(_socket, &fdRead);
		timeval timeout = {1, 0};
		int ret = select(_socket + 1, &fdRead, nullptr, nullptr, &timeout);

		if(ret < 0)
		{
			std::cout << getHintInfor(CLIENT) << "socket: " << _socket << " finished." << std::endl;
			closeSocket();
			return SOCKET_ERROR;
		}
		if(FD_ISSET(_socket, &fdRead))
		{
			FD_CLR(_socket, &fdRead);
			//
		}
		return true;
	}
	return false;
}

bool ClientTCP::isRun()
{
	return _socket != INVALID_SOCKET;
}

const std::string ClientTCP::getHintInfor(Hint hintInfor)
{
	std::time_t nowtime = time(nullptr);
	char tmp[64];
	switch(hintInfor)
	{
	case SERVER:
		strftime(tmp, sizeof(tmp), "[SERVER %H:%M:%S]", localtime(&nowtime));
		break;
	case CLIENT:
		strftime(tmp, sizeof(tmp), "[CLIENT %H:%M:%S]", localtime(&nowtime));
		break;
	default:
		break;
	}
	return tmp;
}
