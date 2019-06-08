#include "servertcp.h"
#include "../common/dataframework.h"
#include <ctime>
#include <arpa/inet.h>

ServerTCP::ServerTCP()
		: _socket(INVALID_SOCKET),
		_recvCount(0)
{
	memset(_recvBuf, 0, sizeof(_recvBuf));	
}

ServerTCP::~ServerTCP()
{
	
}
/**
 * 用于初始化socket，在绑定ip和端口号时，会检查是否初始化完成
 * */
SOCKET ServerTCP::initSocket()
{
	_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //建立服务器套接字
	if(INVALID_SOCKET == _socket)
	{
		std::cerr << getHintInfor(SERVER) << "Server initialized unsuccessfully!" << std::endl;
		closeSocket();
		return SOCKET_ERROR;
	}
	else
		std::cout << getHintInfor(SERVER) << "Server initialized successfully." << std::endl;
	return _socket;
}

/**
 * 绑定服务器的ip和端口号
 * */
int ServerTCP::bindHost(const std::string ip, int port)
{
	if(INVALID_SOCKET == _socket)
		initSocket();
	sockaddr_in _sin = {}; //c++11的初始化，列表初始化
	_sin.sin_family = AF_INET;
	_sin.sin_port = htons(port);
	_sin.sin_addr.s_addr = ip.empty() ? INADDR_ANY : inet_addr(ip.c_str()); //建议使用inet_pton以及inet_ntop
	int ret = bind(_socket, (sockaddr*)&_sin, sizeof(sockaddr_in));
	if(SOCKET_ERROR == ret)
	{
		std::cerr << getHintInfor(SERVER) << "Server bind host unsuccessfully!" << std::endl;
		std::cerr << "\tIP:" << inet_ntoa( _sin.sin_addr) << std::endl;
		std::cerr << "\tPort:" << ntohs(_sin.sin_port) << std::endl;
	}
	else
		std::cout << getHintInfor(SERVER) << "Server bind " << ip << ":" << port << " successfully!" << std::endl;
	return ret;
}

/**
 * 监听主机
 * */
int ServerTCP::listenHost(const int backlog)
{
	int ret = listen(_socket, backlog);
	if(SOCKET_ERROR == ret)
		std::cerr << getHintInfor(SERVER) << "Server listen host unsuccessfully!" << std::endl;
	else
		std::cout << getHintInfor(SERVER) << "Server listen host successfully!" << std::endl;
	return ret;
}

/**
 * 接受客户端的连接，并把客户端存放入向量数组中
 * */
SOCKET ServerTCP::acceptClient()
{
	SOCKET cSock = INVALID_SOCKET;
	sockaddr_in _clientAddr = {};
	int nAddrLen = sizeof(_clientAddr);
	cSock = accept(_socket, (sockaddr*)&_clientAddr, (socklen_t*)&nAddrLen);
	if(INVALID_SOCKET == cSock)
	{
		std::cerr << getHintInfor(SERVER) << "Server connect client failed!" << std::endl;
		return SOCKET_ERROR;
	}
	std::cout << getHintInfor(CLIENT) << "Client " << inet_ntoa(_clientAddr.sin_addr) << ":" << ntohs(_clientAddr.sin_port) << " connected to server!" <<std::endl;

	_clients.push_back(new ClientSocket(cSock));

	return cSock;
}

/**
 * 关闭所有的套接字
 * */
void ServerTCP::closeSocket()
{
	if(_socket != INVALID_SOCKET)
	{
		for(size_t index = 0; index < _clients.size(); ++index)
		{
			close(_clients[index]->sockfd());
			delete _clients[index];
			_clients[index] = nullptr;
		}
		close(_socket);
		_clients.clear();
	}
}

bool ServerTCP::isRun()
{
	return _socket != INVALID_SOCKET;
}

bool ServerTCP::selectRun()
{
	if(!isRun())
		return false;
	
	fd_set fdRead;
	fd_set fdWrite;
	fd_set fdExp;

	FD_ZERO(&fdRead);
	FD_ZERO(&fdWrite);
	FD_ZERO(&fdExp);

	FD_SET(_socket, &fdRead);
	FD_SET(_socket, &fdWrite);
	FD_SET(_socket, &fdExp);

	SOCKET maxSocket = _socket;

	for(size_t index = 0; index < _clients.size(); ++index)
	{
		FD_SET(_clients[index]->sockfd(), &fdRead);
		maxSocket = (maxSocket < _clients[index]->sockfd()) ? _clients[index]->sockfd() : maxSocket;
	}

	timeval timeout = {1, 0};

	int ret = select(maxSocket + 1, &fdRead, &fdWrite, &fdExp, &timeout);
	if(ret < 0)
	{
		std::cout << getHintInfor(SERVER) << "select start failed." << std::endl;
		closeSocket();
		return false;
	}
	if(FD_ISSET(_socket, &fdRead))
	{
		FD_CLR(_socket, &fdRead);
		acceptClient();
	}

	for(size_t index = 0; index < _clients.size(); ++index)
	{
		if(FD_ISSET(_clients[index]->sockfd(), &fdRead))
		{
			if(-1 == recvData(_clients[index]))
			{
				auto iter = _clients.begin() + index;
				if(iter != _clients.end())
				{
					delete _clients[index];
					_clients[index] = nullptr;
					_clients.erase(iter);
				}
			}
		}
	}
	return true;
}

int ServerTCP::recvData(ClientSocket *pClient)
{
	int recvLen = recv(pClient->sockfd(), _recvBuf, sizeof(_recvBuf), 0);
	if(recvLen <= 0)
	{
		std::cout << getHintInfor(SERVER) << "Client " << pClient->sockfd() << " closed." <<std::endl;
		return -1;
	}

	//处理粘包、少包，进行拆包
	memcpy(pClient->msgBuf() + pClient->getLastPos(), _recvBuf, recvLen);
	pClient->setLastPos(pClient->getLastPos() + recvLen);
	while(pClient->getLastPos() >= sizeof(Header))
	{
		Header *data = (Header*)(pClient->msgBuf());
		if(pClient->getLastPos() >= data->length)
		{
			int nSize = pClient->getLastPos() - data->length;
			//处理数据
			onNetMsg(pClient->sockfd(), data);
			memcpy(pClient->msgBuf(), pClient->msgBuf() + data->length, nSize);
			pClient->setLastPos(nSize);
		}
		else
			break;
	}
	return 1;
}

void ServerTCP::onNetMsg(SOCKET _cSock, Header *data)
{
	switch(data->cmd)
	{
	case CMD_LOGIN:
		std::cout << getHintInfor(CLIENT) << "==> (LOGIN)" << std::endl;
		break;
	case CMD_LOGIN_RESULT:
		break;
	case CMD_LOGOUT:
		break;
	case CMD_LOGOUT_RESULT:
		break;
	case CMD_NEW_USER:
		break;
	case CMD_ERROR:
		break;
	}
}

int ServerTCP::sendData(SOCKET _cSock, Header *data)
{
	if(isRun() && data)
		return send(_socket, (const char*)data, data->length, 0);
	return SOCKET_ERROR;
}

void ServerTCP::sendData2All(Header *data)
{
	if(!data)
		return;
	for(size_t index = 0; index < _clients.size(); ++index)
			sendData(_clients[index]->sockfd(), data);
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


