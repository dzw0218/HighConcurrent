#include "servertcp.h"
#include <ctime>

ServerTCP::ServerTCP()
		: _socket(INVALID_SOCKET),
		oneFlag(true)
{
	
}

ServerTCP::~ServerTCP()
{
	closeSocket();
}

SOCKET ServerTCP::initSocket()
{
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(SOCKET_ERROR == _socket)
	{
		std::cout << getHintInfor(SERVER) << "socket created failed." << std::endl;
		return INVALID_SOCKET;
	}
	std::cout << getHintInfor(SERVER) << "socket created successfully." << std::endl;
	return _socket;
}

int ServerTCP::bindServer(const std::string ip, const unsigned port)
{
	if(INVALID_SOCKET == _socket)
		initSocket();

	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(port);
	_sin.sin_addr.s_addr = ip.empty() ? INADDR_ANY : inet_addr(ip.c_str());
	int ret = bind(_socket, (sockaddr*)&_sin, sizeof(sockaddr_in));
	
	if(SOCKET_ERROR == ret)
	{
		std::cout << getHintInfor(SERVER) << "bind server failed." << std::endl;
		return SOCKET_ERROR;
	}
	std::cout << getHintInfor(SERVER) << "bind server " << ip << ":" << port << " successfully." << std::endl;
	return ret;
}

int ServerTCP::listenServer(const int backlog)
{
	int ret = listen(_socket, backlog);
	if(ret < 0)
	{
		std::cout << getHintInfor(SERVER) << "listen to server failed." << std::endl;
		return -1;
	}
	else
		std::cout << getHintInfor(SERVER) << "listen to server successfully." << std::endl;
	return ret;
}

SOCKET ServerTCP::acceptClient()
{
	SOCKET _cSock = INVALID_SOCKET;
	sockaddr_in _sin = {};
	int nAddrLen = sizeof(_sin);
	_cSock = accept(_socket, (sockaddr*)&_sin, (socklen_t*)&nAddrLen);
	
	if(INVALID_SOCKET == _cSock)
	{
		std::cout << getHintInfor(SERVER) << "connect to client failed." << std::endl;
		return SOCKET_ERROR;
	}
	std::cout << getHintInfor(CLIENT) << "client " << inet_ntoa(_sin.sin_addr) << ":" << ntohs(_sin.sin_port) << " connected to server" << std::endl;
	_clients.push_back(new ClientSocket(_cSock));
	return _cSock;
}

void ServerTCP::closeSocket()
{
	if(_socket != INVALID_SOCKET)
	{
		for(size_t index = 0; index < _clients.size(); ++index)
		{
			delete _clients[index];
			_clients[index] = nullptr;
		}
		close(_socket);
		_clients.clear();
	}
}

bool ServerTCP::epollRun()
{
	if(!isRun())
		return false;

	if(oneFlag)
	{
		oneFlag = false;
		efd = epoll_create(MAX_CLIENT);
		if(-1 == efd)
		{
			std::cout << getHintInfor(SERVER) << "epoll create failed." << std::endl;
			return false;
		}
		tep.events = EPOLLIN;
		tep.data.fd = _socket;
		int ret = epoll_ctl(efd, EPOLL_CTL_ADD, _socket, &tep);
		if(-1 == ret)
		{
			std::cout << getHintInfor(SERVER) << "server add to epoll failed." << std::endl;
			return false;
		}
		return true;
	}
	else
	{
		int nReady = epoll_wait(efd, ep, AMX_CLIENT, -1);
		if(-1 == nReady)
		{
			std::cout << getHintInfor(SERVER) << "server failed to wait for connection." << std::endl;
			return false;
		}

		for(int index = 0; index < nReady; ++index)
		{
			if(!(ep[index].events & EPOLLIN))
				continue;
			if(ep[index].data.fd == _socket)
			{
				SOCKET _cSock = acceptClient();
				tep.events = EPOLLIN;
				tep.data.fd = _cSock;
				int ret = epoll_ctl(efd, EPOLL_CTL_ADD, _cSock, &tep);
				if(-1 == ret)
					std::cout << getHintInfor(SERVER) << "add client to epoll failed." << std::endl;
			}
			else
			{
				
			}
		}
	}
}

bool ServerTCP::isRun()
{
	return _socket != INVALID_SOCKET;
}

int ServerTCP::recvData(ClientSocket *pClient)
{
	int nSize = recv(pClient->getSockfd(), _recvBuf, sizeof(_recvBuf), 0);
	if(nSize <= 0)
	{
		std::cout << getHintInfor(CLIENT) << "client has been closed." << std::endl;
		return -1;
	}

	memcpy(pClient->getMsgBuf() + pClient->getLastPos(), _recvBuf, nSize);
	pClient->setLastPos(pClient->getLastPos() + nSize);
	while(pClient->getLastPos() >= sizeof(Header))
	{
		Header *data = (Header*)(pClient->getMsgBuf());
		if(pClient->getLastPos() >= data->length)
		{
			int lastSize = pClient->getLastPos() - data->length;
			//
			memcpy(pClient->getMsgBuf(), pClient->getMsgBuf() + data->length, lastSize);
			pClient->setLastPos(lastSize);
		}
		else
			break;
	}
	return 1;
}

void ServerTCP::onNetMsg(Header *data)
{
	
}

int ServerTCP::sendData(SOCKET _cSock, Header *data)
{
	if(!isRun() || !data)
		return -1;
	return send(_cSock, (const char*)data, data->length, 0);
}

void ServerTCP::sendData2All(Header *data)
{
	if(!data)
		return;
	for(size_t index = 0; index < _clients.size(); ++index)
		send(_clients[index]->getSockfd(), (const char*)data, data->length, 0);
}

/**
 * 用于获取当前时间，作为提示信息的组成部分，或用于日志
 * */
const std::string ServerTCP::getHintInfor(Hint hintInfor)
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
