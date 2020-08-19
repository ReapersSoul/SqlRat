#include "Multi_NetworkHandlerServer.h"

bool Multi_NetworkHandlerServer::IsConnected()
{
	for (int i = 0; i < ConnectedSockets.size(); i++) {
		if (ConnectedSockets[i].connected) {
			return true;
		}
	}
	return false;
}

bool Multi_NetworkHandlerServer::Init(PCSTR Port)
{
	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed: %d\n", iResult);
		return false;
	}
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, Port, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return false;
	}

	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return false;
	}
	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}
	freeaddrinfo(result);
	return true;
}

bool Multi_NetworkHandlerServer::Listen()
{
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		return false;
	}

	return true;
}

bool Multi_NetworkHandlerServer::AcceptConn()
{
	SOCKET ConnectedSocket = INVALID_SOCKET;

	// Accept a client socket
	ConnectedSocket = accept(ListenSocket, NULL, NULL);
	if (ConnectedSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		return false;
	}
	ConnectedSockets.push_back(NamedSOCKET(ConnectedSocket, ""));

	RecvDataT<std::string>(&ConnectedSockets[ConnectedSockets.size() - 1].name, &ConnectedSockets[ConnectedSockets.size()-1]);

	ClientThreads.push_back(std::thread(*RecvFunct, &ConnectedSockets[ConnectedSockets.size()-1]));

	SendDataT<std::string>(Name, &ConnectedSockets[ConnectedSockets.size()-1]);

	return true;
}

bool Multi_NetworkHandlerServer::DisConnect(NamedSOCKET * ns)
{
	// shutdown the send half of the connection since no more data will be sent
	iResult = shutdown(ns->sock, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ns->sock);
		return false;
	}

	// cleanup
	closesocket(ns->sock);

	ns->connected = false;
	ClientThreads[FindSocketIndex(*ns)].join();

	return true;
}

bool Multi_NetworkHandlerServer::DefaultInitConnect()
{
	if (Init("27015")) {
		if (Listen()) {
			if (AcceptConn()) {
				return true;
			}
		}
	}

	return false;
}
