#ifndef DATAFRAMEWORK_H_
#define DATAFRAMEWORK_H_

#include <iostream>
#include <string>

enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGOUT,
	CMD_LOGOUT_RESULT,
	CMD_NEW_USER,
	CMD_ERROR
};

struct Header
{
	unsigned int length;
	int cmd;
};

struct Login : public Header
{
	Login()
	{
		length = sizeof(Login);
		cmd = CMD_LOGIN;
	}
	// data
};

struct LoginResult : public Header
{
	LoginResult()
	{
		length = sizeof(LoginResult);
		cmd = CMD_LOGIN_RESULT;
	}
};

struct Logout : public Header
{
	Logout()
	{
		length = sizeof(Logout);
		cmd = CMD_LOGOUT;
	}
};

struct LogoutResult : public Header
{
	LogoutResult()
	{
		length = sizeof(LogoutResult);
		cmd = CMD_LOGOUT_RESULT;
	}
};

struct NewUserJoin : public Header
{
	NewUserJoin()
	{
		length = sizeof(NewUserJoin);
		cmd = CMD_NEW_USER;
	}
};

struct LogError : public Header
{
	LogError()
	{
		length = sizeof(LogError);
		cmd = CMD_ERROR;
	}
};

#endif // DATAFRAMEWORK_H_
