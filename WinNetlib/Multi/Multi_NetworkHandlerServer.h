#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")


#include <vector>
#include <string>
#include <iostream>
#include <thread>
#include "Multi_NetworkBase.h"

class Multi_NetworkHandlerServer: public Multi_NetworkBase
{
public:

	bool IsConnected();

	std::vector<std::thread> ClientThreads;

	void (*RecvFunct)(NamedSOCKET * ns);

	bool Init(PCSTR Port);

	bool Listen();

	bool AcceptConn();

	bool DisConnect(NamedSOCKET * ns);

	bool DefaultInitConnect();

	~Multi_NetworkHandlerServer()
	{
		for (int i = 0; i < ClientThreads.size(); i++)
		{
			ClientThreads[i].join();
		}
	}
};

