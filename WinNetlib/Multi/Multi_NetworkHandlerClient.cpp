#include "Multi_NetworkHandlerClient.h"


bool Multi_NetworkHandlerClient::Connect(PCSTR ip, PCSTR Port)
{
    if (Init(ip, Port)) {
        if (Connect()) {
            ConnectedSockets[ConnectedSockets.size() - 1].IP = ip;
            ConnectedSockets[ConnectedSockets.size() - 1].PORT = Port;
            return true;
        }
    }
    return false;
}

bool Multi_NetworkHandlerClient::Init(PCSTR ip,PCSTR Port)
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
        return false;
    }
    return true;
}

bool Multi_NetworkHandlerClient::Connect()
{
    // Attempt to connect to the first address returned by
// the call to getaddrinfo
    ptr = result;

    // Create a SOCKET for connecting to server
    SOCKET ConnectedSocket = socket(ptr->ai_family, ptr->ai_socktype,
        ptr->ai_protocol);

    if (ConnectedSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return false;
    }

    // Connect to server.
    iResult = connect(ConnectedSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(ConnectedSocket);
        ConnectedSocket = INVALID_SOCKET;
    }

    // Should really try the next address returned by getaddrinfo
    // if the connect call failed
    // But for this simple example we just free the resources
    // returned by getaddrinfo and print an error message

    freeaddrinfo(result);

    if (ConnectedSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return false;
    }
    ConnectedSockets.push_back(NamedSOCKET(ConnectedSocket, ""));
    

    SendDataT<std::string>(Name, &ConnectedSockets[ConnectedSockets.size()-1]);

    RecvDataT<std::string>(&ConnectedSockets[ConnectedSockets.size() - 1].name, &ConnectedSockets[ConnectedSockets.size()-1]);

    ServerThreads.push_back(std::thread(*RecvFunct, &ConnectedSockets[ConnectedSockets.size() - 1]));

    return true;
}

bool Multi_NetworkHandlerClient::DisConnect(NamedSOCKET * ns)
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
    ServerThreads[FindSocketIndex(*ns)].join();

    return true;
}

bool Multi_NetworkHandlerClient::DefaultInitConnect()
{
    if (Init("127.0.0.1", "27015")) {
        if (Connect()) {
            ConnectedSockets[ConnectedSockets.size() - 1].IP = "127.0.0.1";
            ConnectedSockets[ConnectedSockets.size() - 1].PORT = "27015";
            return true;
        }
    }
    return false;
}
