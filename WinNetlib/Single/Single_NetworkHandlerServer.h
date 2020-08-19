#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")


#include <vector>
#include <string>
#include <iostream>
#include "Single_NetworkBase.h"

class Single_NetworkHandlerServer: public Single_NetworkBase
{
	bool connected=false;
public:

	std::string GetConnectedName();

	bool GetConnected();

	bool IsConnected();

	bool Init(PCSTR Port);

	bool Listen();

	bool AcceptConn();

	bool DisConnect();

	bool DefaultInitConnect();

	bool InitConnect(std::string Port);

	~Single_NetworkHandlerServer();
};

