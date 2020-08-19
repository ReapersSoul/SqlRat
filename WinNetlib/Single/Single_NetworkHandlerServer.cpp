#include "Single_NetworkHandlerServer.h"

std::string Single_NetworkHandlerServer::GetConnectedName()
{
	return ConnectedSocket.name;
}

bool Single_NetworkHandlerServer::GetConnected()
{
	return ConnectedSocket.connected;
}

bool Single_NetworkHandlerServer::IsConnected()
{
	return connected;
}

bool Single_NetworkHandlerServer::Init(PCSTR Port)
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
	this->port = Port;
	return true;
}

bool Single_NetworkHandlerServer::Listen()
{
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return false;
	}

	return true;
}

bool Single_NetworkHandlerServer::AcceptConn()
{
	ConnectedSocket.sock = INVALID_SOCKET;

	// Accept a client socket
	ConnectedSocket.sock = accept(ListenSocket, NULL, NULL);
	if (ConnectedSocket.sock == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		connected = false;
		return false;
	}

	RecvDataT<std::string>(&ConnectedSocket.name);

	SendDataT<std::string>(Name);

	ConnectedSocket.connected = true;

	return true;
}

bool Single_NetworkHandlerServer::DisConnect()
{
	// shutdown the send half of the connection since no more data will be sent
	iResult = shutdown(ConnectedSocket.sock, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ConnectedSocket.sock);
		WSACleanup();
		return false;
	}

	// cleanup
	closesocket(ConnectedSocket.sock);
	WSACleanup();
	ConnectedSocket.connected = false;

	return true;
}

bool Single_NetworkHandlerServer::DefaultInitConnect()
{
	if (Init("27015")) {
		if (Listen()) {
			if (AcceptConn()) {
				this->port = "27015";
				return true;
			}
		}
	}

	return false;
}

bool Single_NetworkHandlerServer::InitConnect(std::string Port)
{
	if (Init((PCSTR)Port.c_str())) {
		if (Listen()) {
			if (AcceptConn()) {
				this->port = Port;
				return true;
			}
		}
	}

	return false;
}

Single_NetworkHandlerServer::~Single_NetworkHandlerServer()
{
}
