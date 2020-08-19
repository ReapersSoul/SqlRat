#include "Single_NetworkHandlerClient.h"


std::string Single_NetworkHandlerClient::GetConnectedName()
{
    return ConnectedSocket.name;
}

bool Single_NetworkHandlerClient::Init(PCSTR ip,PCSTR Port)
{

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return false;
    }
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(ip, Port, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return false;
    }
    this->ip = ip;
    this->port = Port;
    return true;
}

bool Single_NetworkHandlerClient::IsConnected()
{
    return connected;
}

bool Single_NetworkHandlerClient::Connect()
{
    // Attempt to connect to the first address returned by
// the call to getaddrinfo
    ptr = result;

    // Create a SOCKET for connecting to server
    ConnectedSocket.sock = socket(ptr->ai_family, ptr->ai_socktype,
        ptr->ai_protocol);

    if (ConnectedSocket.sock == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        connected = false;
        return false;
    }

    // Connect to server.
    iResult = connect(ConnectedSocket.sock, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(ConnectedSocket.sock);
        ConnectedSocket.sock = INVALID_SOCKET;
    }

    // Should really try the next address returned by getaddrinfo
    // if the connect call failed
    // But for this simple example we just free the resources
    // returned by getaddrinfo and print an error message

    freeaddrinfo(result);

    if (ConnectedSocket.sock == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        connected = false;
        return false;
    }

    SendDataT<std::string>(Name);

    RecvDataT<std::string>(&ConnectedSocket.name);

    connected = true;
    return true;
}

bool Single_NetworkHandlerClient::DisConnect()
{
    // shutdown the send half of the connection since no more data will be sent
    iResult = shutdown(ConnectedSocket.sock, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ConnectedSocket.sock);
        WSACleanup();
        ip = "";
        port = "";
        connected = false;
        return false;
    }
    
    // cleanup
    closesocket(ConnectedSocket.sock);
    WSACleanup();
    ip = "";
    port = "";
    connected = false;
    return true;
}

bool Single_NetworkHandlerClient::DefaultInitConnect()
{
    if (Init("127.0.0.1", "27015")) {
        if (Connect()) {
            this->ip = "127.0.0.1";
            this->port = "27015";
            return true;
        }
    }
    return false;
}
