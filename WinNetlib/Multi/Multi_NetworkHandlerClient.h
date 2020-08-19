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

class Multi_NetworkHandlerClient: public Multi_NetworkBase 
{
public:
	std::vector<std::thread> ServerThreads;

	void (*RecvFunct)(NamedSOCKET * ns);

	bool Connect(PCSTR ip, PCSTR Port);

	bool Init(PCSTR ip, PCSTR Port);

	bool Connect();

	bool DisConnect(NamedSOCKET * ns);

	bool DefaultInitConnect();

	~Multi_NetworkHandlerClient()
	{
		for (int i = 0; i<ConnectedSockets.size(); i++) {
			DisConnect(&ConnectedSockets[i]);
		}
	}

};

