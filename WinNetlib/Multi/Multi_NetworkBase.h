#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")


#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
//#include <opencv2/opencv.hpp>
#include <map>

struct DataType
{
	DataType() {
		Name = "";
		Identity = 0;
	}

	DataType(std::string name, int identity) {
		Name = name;
		Identity = identity;
	}

	std::string Name = "";
	int Identity = 0;

	bool operator==(DataType et) {
		return (this->Identity == et.Identity || this->Name == et.Name);
	}
	bool operator!=(DataType et) {
		return (this->Identity != et.Identity || this->Name != et.Name);
	}

};

struct DataTypeList {
private:
	std::vector<DataType> Types;
	int FindTypeIndex(DataType dt) {
		for (int i = 0; i < Types.size(); i++) {
			if (Types[i] == dt) {
				return i;
			}
		}
		return NULL;
	}
public:

	DataTypeList() {
		AddType("Invalid", 0);
		AddType("DataTypeList", 1);
	}

	DataType FindType(DataType dt) {
		for (int i = 0; i < Types.size(); i++) {
			if (Types[i] == dt) {
				return Types[i];
			}
		}
		return DataType();
	}

	DataType FindType(std::string s) {
		for (int i = 0; i < Types.size(); i++) {
			if (Types[i].Name == s) {
				return Types[i];
			}
		}
		return DataType();
	}

	DataType FindType(int identity) {
		for (int i = 0; i < Types.size(); i++) {
			if (Types[i].Identity == identity) {
				return Types[i];
			}
		}
		return DataType();
	}

	bool TypeExists(DataType dt) {
		DataType t = FindType(dt);
		if (t.Name == "" && t.Identity == 0) {
			return false;
		}
		else {
			return true;
		}
	}

	bool TypeExists(std::string s) {
		DataType t = FindType(s);
		if (t.Name == "" && t.Identity == 0) {
			return false;
		}
		else {
			return true;
		}
	}

	bool TypeExists(int identity) {
		DataType t = FindType(identity);
		if (t.Name == "" && t.Identity == 0) {
			return false;
		}
		else {
			return true;
		}
	}
	//returns true if type has been added and false if the type already exists
	bool AddType(DataType dt) {
		if (!TypeExists(dt)) {
			Types.push_back(dt);
			return true;
		}
		else {
			return false;
		}
	}
	//returns true if type has been added and false if the type already exists
	bool AddType(std::string name, int identity) {
		if (!TypeExists(DataType(name, identity))) {
			Types.push_back(DataType(name, identity));
			return true;
		}
		else {
			return false;
		}
	}

	//returns true if type has been removed and false if the type didnt exists
	bool RemoveType(DataType dt) {
		if (!TypeExists(dt)) {
			Types.erase(Types.begin() + FindTypeIndex(dt));
			return true;
		}
		else {
			return false;
		}
	}
	//returns true if type has been removed and false if the type didnt exists
	bool RemoveType(std::string name, int identity) {
		if (!TypeExists(DataType(name, identity))) {
			Types.erase(Types.begin() + FindTypeIndex(DataType(name, identity)));
			return true;
		}
		else {
			return false;
		}
	}

	int size() {
		return Types.size();
	}

	DataType operator[](int identity) {
		return FindType(identity);
	}

	DataType operator[](std::string name) {
		return FindType(name);
	}

	typedef typename std::vector<DataType> array_type;
	typedef typename array_type::iterator iterator;
	typedef typename array_type::const_iterator const_iterator;

	inline iterator begin() noexcept { return Types.begin(); }
	inline const_iterator cbegin() const noexcept { return Types.cbegin(); }
	inline iterator end() noexcept { return Types.end(); }
	inline const_iterator cend() const noexcept { return Types.cend(); }
};

struct NamedSOCKET {

	NamedSOCKET() {
		sock = INVALID_SOCKET;
		name = "";
	}

	NamedSOCKET(SOCKET s, std::string Name) {
		sock = s;
		name = Name;
	}

	bool operator==(NamedSOCKET ns) {
		if (this->name==ns.name&&this->sock==ns.sock) {
			return true;
		}
		else {
			return false;
		}
	}

	bool connected;

	std::string IP;
	std::string PORT;

	SOCKET sock;
	std::string name;
};


class Multi_NetworkBase
{
protected:

	std::string Name;

	WSADATA wsaData;
	int iResult;

	struct addrinfo* result = NULL, * ptr = NULL, hints;

	SOCKET ListenSocket = INVALID_SOCKET;

	DataTypeList DataTypes;

public:
	std::vector<NamedSOCKET> ConnectedSockets;

	void SetName(std::string s) {
		Name = s;
	}

	char* addToEnd(char* first, int firstsize, char* second, int secondsize) {
		char* ret;
		ret = new char[firstsize + secondsize];
		for (int i = 0; i < firstsize; i++)
		{
			ret[i] = first[i];
		}
		for (int i = 0; i < secondsize; i++)
		{
			ret[firstsize + i] = second[i];
		}
		return ret;
	}

	char* addNullToEnd(char* src, int size) {
		char* ret = src;

		ret[size] = (char)'\0';

		return ret;
	}

	char* PullPart(char* src, int start, int bytesToPull) {
		char* ret;
		ret = new char[bytesToPull];
		for (int i = 0; i < bytesToPull; i++)
		{
			ret[i] = src[i + start];
		}
		return ret;
	}

	NamedSOCKET * FindSocket(NamedSOCKET ns) {
		for (int i = 0; i < ConnectedSockets.size(); i++)
		{
			if (ns == ConnectedSockets[i]) {
				return &ConnectedSockets[i];
			}
		}
	}

	NamedSOCKET * FindSocket(std::string s,bool IsName) {
		if (IsName) {
			for (int i = 0; i < ConnectedSockets.size(); i++)
			{
				if (s == ConnectedSockets[i].name) {
					return &ConnectedSockets[i];
				}
			}
		}
		else {
			for (int i = 0; i < ConnectedSockets.size(); i++)
			{
				if (s == ConnectedSockets[i].IP) {
					return &ConnectedSockets[i];
				}
			}
		}
	}

	NamedSOCKET * FindSocket(SOCKET s) {
		for (int i = 0; i < ConnectedSockets.size(); i++)
		{
			if (s == ConnectedSockets[i].sock) {
				return &ConnectedSockets[i];
			}
		}
	}

	int FindSocketIndex(NamedSOCKET ns) {
		for (int i = 0; i < ConnectedSockets.size(); i++)
		{
			if (ns == ConnectedSockets[i]) {
				return i;
			}
		}
	}

	int FindSocketIndex(std::string name) {
		for (int i = 0; i < ConnectedSockets.size(); i++)
		{
			if (name == ConnectedSockets[i].name) {
				return i;
			}
		}
	}

	int FindSocketIndex(SOCKET s) {
		for (int i = 0; i < ConnectedSockets.size(); i++)
		{
			if (s == ConnectedSockets[i].sock) {
				return i;
			}
		}
	}

	bool SendData(char* data, int size, NamedSOCKET * ns)
	{
		//send size
		iResult = send(ns->sock, reinterpret_cast<char*>(&size), sizeof(int), 0);
		if (iResult == SOCKET_ERROR) {
			//printf("send failed: %d\n", WSAGetLastError());
			return false;
		}

		//send data
		iResult = send(ns->sock, data, size, 0);
		if (iResult == SOCKET_ERROR) {
			//printf("send failed: %d\n", WSAGetLastError());
			return false;
		}
		Sleep(100);
		return true;
	}

	template<typename T>
	bool SendDataT(T t, NamedSOCKET * ns) {
		return SendData(reinterpret_cast<char*>(&t), sizeof(T),ns);
	}

	template<>
	bool SendDataT<std::string>(std::string s, NamedSOCKET * ns) {
		return SendData((char*)s.c_str(), s.size() + 1,ns);
	}

	template<>
	bool SendDataT<std::wstring>(std::wstring s, NamedSOCKET * ns) {
		return SendData((char*)s.c_str(), s.capacity()*sizeof(wchar_t),ns);
	}


	bool RecvData(char** data, NamedSOCKET * ns)
	{


		int size = 0;

		iResult = recv(ns->sock, reinterpret_cast<char*>(&size), sizeof(int), 0);
		if (iResult > 0) {
			//printf("Bytes received: %d\n", iResult);
			//std::cout << "size of packet: " << size << std::endl;
		}
		else if (iResult == 0) {
			printf("Connection closed\n");
			ns->connected = false;
			return false;
		}
		else {
			//printf("recv failed: %d\n", WSAGetLastError());
			return false;
		}

		char* ret = new char[size];

		iResult = recv(ns->sock, ret, size, 0);
		if (iResult > 0) {
			//printf("Bytes received: %d\n", iResult);
		}
		else if (iResult == 0) {
			printf("Connection closed\n");
			ns->connected = false;
			return false;
		}
		else {
			//printf("recv failed: %d\n", WSAGetLastError());
			return false;
		}

		*data = ret;

		Sleep(300);
		return true;
	}

	template<typename T>
	bool RecvDataT(T * item, NamedSOCKET * ns)
	{
		char* data = nullptr;

		if (!RecvData(&data, ns)) {
			return NULL;
		}

		T* ret = reinterpret_cast<T*>(data);

		*item = *ret;
	}

	template<>
	bool RecvDataT<std::string>(std::string * s, NamedSOCKET * ns)
	{
		char* data = nullptr;

		if (RecvData(&data,ns)) {

			*s = data;
			return true;
		}
		return false;
	}

	template<>
	bool RecvDataT<std::wstring>(std::wstring* s, NamedSOCKET * ns)
	{
		char* data = nullptr;

		if (RecvData(&data,ns)) {

			*s = (wchar_t *)data;
			return true;
		}
		return false;
	}

	bool SendBatchData(char* data, int size, int packetSize, NamedSOCKET * ns) {
		int numOfPackets = size / packetSize;
		int lastPackSize = size % packetSize;


		SendDataT<int>(size,ns);
		SendDataT<int>(numOfPackets,ns);
		SendDataT<int>(packetSize,ns);
		SendDataT<int>(lastPackSize,ns);

		int cursor = 0;
		for (int i = 0; i < numOfPackets; i++)
		{
			//send porion of data
			char* tmp = PullPart(data, cursor, packetSize);
			SendData(tmp, packetSize,ns);
			cursor += packetSize;
			printf("Send Progress: %%%4.2f\n", ((double)cursor / (double)size) * 100);
		}
		if (lastPackSize > 0) {
			//if extra info send here
			char* tmp = PullPart(data, cursor, lastPackSize);
			SendData(tmp, lastPackSize,ns);
			cursor += lastPackSize;
			printf("Send Progress: %%%4.2f\n", ((double)cursor / (double)size) * 100);
		}
		return true;
	}

	bool RecvBatchData(char** data, NamedSOCKET * ns) {

		int size;
		if (!RecvDataT<int>(&size,ns)) {
			return false;
		}
		int numOfPackets;
		if (!RecvDataT<int>(&numOfPackets,ns)) {
			return false;
		}
		int packetSize;
		if (!RecvDataT<int>(&packetSize,ns)) {
			return false;
		}
		int lastPackSize;
		if (!RecvDataT<int>(&lastPackSize,ns)) {
			return false;
		}

		char* ret;
		ret = new char[(numOfPackets * packetSize) + lastPackSize];

		int currentSize = 0;

		for (int i = 0; i < numOfPackets; i++)
		{
			//recv portion of data
			char* tmp;
			if (RecvData(&tmp,ns)) {
				ret = addToEnd(ret, currentSize, tmp, packetSize);
				currentSize += packetSize;
				printf("Recive Progress: %%%4.2f\n", ((double)currentSize / (double)size) * 100);
			}
			else {
				return false;
			}

		}
		if (lastPackSize > 0) {
			//if extra info recv here
			char* tmp;
			tmp = new char[lastPackSize];
			if (RecvData(&tmp, ns)) {
				ret = addToEnd(ret, currentSize, tmp, lastPackSize);
				currentSize += lastPackSize;
				printf("Recive Progress: %%%4.2f\n", ((double)currentSize / (double)size) * 100);
			}
			else {
				return false;
			}
		}
		ret = addNullToEnd(ret, currentSize);
		*data = ret;
		return true;
	}

	bool SendFile(std::string path, int PackSize, NamedSOCKET * ns) {
		std::ifstream in(path, std::ios::binary);
		if (in.is_open()) {

			in.seekg(0, in.end);
			int length = in.tellg();
			in.seekg(0, in.beg);

			if (length == 0) {
				printf("empty file");
				in.close();
				return false;
			}

			int numOfPackets = length / PackSize;
			int lastPackSize = length % PackSize;


			SendDataT<int>(length, ns);
			SendDataT<int>(PackSize, ns);
			SendDataT<int>(numOfPackets, ns);
			SendDataT<int>(lastPackSize, ns);

			int bytesSent = 0;

			char* packet;
			packet = new char[PackSize];
			for (int i = 0; i < numOfPackets; i++)
			{
				in.read(packet, PackSize);
				SendData(packet, PackSize, ns);
				bytesSent += PackSize;
				printf("Send Progress: %%%4.2f\n", ((double)bytesSent / (double)length) * 100);
			}
			if (lastPackSize > 0) {
				in.read(packet, PackSize);
				SendData(packet, lastPackSize,ns);
				bytesSent += lastPackSize;
				printf("Send Progress: %%%4.2f\n", ((double)bytesSent / (double)length) * 100);
			}
			return true;
		}
		return false;
	}

	bool RecvFile(std::string path, NamedSOCKET * ns) {
		std::ofstream out(path, std::ios::binary);

		if (out.is_open() && out.good()) {
			int length;
			if (!RecvDataT<int>(&length,ns)) {
				return false;
			}
			int PackSize;
			if (!RecvDataT<int>(&PackSize, ns)) {
				return false;
			}
			int numOfPackets;
			if (!RecvDataT<int>(&numOfPackets,ns)) {
				return false;
			}
			int lastPackSize;
			if (!RecvDataT<int>(&lastPackSize, ns)) {
				return false;
			}

			int bytesRecived = 0;

			for (int i = 0; i < numOfPackets; i++)
			{
				char* packet;
				packet = new char[PackSize];
				RecvData(&packet, ns);
				out.write(packet, PackSize);
				bytesRecived += PackSize;
				printf("Recive Progress: %%%4.2f\n", ((double)bytesRecived / (double)length) * 100);
			}
			if (lastPackSize > 0) {
				char* packet;
				packet = new char[lastPackSize];
				RecvData(&packet, ns);
				out.write(packet, lastPackSize);
				bytesRecived += lastPackSize;
				printf("Recive Progress: %%%4.2f\n", ((double)bytesRecived / (double)length) * 100);
			}
			out.close();
			return true;
		}
		return false;
	}

	bool SendTypeList(NamedSOCKET * ns) {
		if(!SendDataT<int>(DataTypes.size(),ns))
			return false;
		for (std::vector<DataType>::iterator it = DataTypes.begin(); it != DataTypes.end(); ++it) {
			SendDataT<DataType>(*it,ns);
		}
		return true;
	}

	bool RecvTypeList(NamedSOCKET * ns) {
		int size;
		if (!RecvDataT<int>(&size,ns)) {
			return false;
		}
		for (int i = 0; i < size; i++)
		{
			DataType dt;
			if (!RecvDataT<DataType>(&dt,ns)) {
				return false;
			}

			DataTypes.AddType(dt);
		}
		return true;
	}

	bool SendDataType(DataType s, NamedSOCKET * ns) {
		if(DataTypes.TypeExists(s))
			return SendDataT<int>(s.Identity,ns);
	}

	bool SendDataType(std::string s, NamedSOCKET * ns) {
		if (DataTypes.TypeExists(s))
			return SendDataT<int>(DataTypes.FindType(s).Identity,ns);
	}

	bool SendDataType(int s, NamedSOCKET * ns) {
		if (DataTypes.TypeExists(s))
			return SendDataT<int>(DataTypes.FindType(s).Identity,ns);
	}

	bool SendDataType(std::string s, int identity, NamedSOCKET * ns) {
		if (DataTypes.TypeExists(DataType(s,identity)))
			return SendDataT<int>(DataTypes.FindType(DataType(s, identity)).Identity,ns);
	}

	bool RecvDataType(DataType * s, NamedSOCKET * ns) {
		int identity;
		RecvDataT<int>(&identity,ns);
		if (DataTypes.TypeExists(identity)) {
			*s = DataTypes.FindType(identity);
			return true;
		}
		else {
			*s = DataTypes.FindType(0);
			return false;
		}
	}

	//send recv to from all functions

	bool SendDataToAll(char* data, int size)
	{
		for (int i = 0; i < ConnectedSockets.size(); i++)
		{
			if (!SendData(data, size, &ConnectedSockets[i])) {
				return false;
			}
		}
		return true;
	}

	template<typename T>
	bool SendDataTToAll(T t) {
		return SendDataToAll(reinterpret_cast<char*>(&t), sizeof(T));
	}

	template<>
	bool SendDataTToAll<std::string>(std::string s) {
		return SendDataToAll((char*)s.c_str(), s.size() + 1);
	}

	template<>
	bool SendDataTToAll<std::wstring>(std::wstring s) {
		return SendDataToAll((char*)s.c_str(), s.capacity() * sizeof(wchar_t));
	}

	bool SendBatchDataToAll(char* data, int size, int packetSize) {
		int numOfPackets = size / packetSize;
		int lastPackSize = size % packetSize;


		SendDataTToAll<int>(size);
		SendDataTToAll<int>(numOfPackets);
		SendDataTToAll<int>(packetSize);
		SendDataTToAll<int>(lastPackSize);

		int cursor = 0;
		for (int i = 0; i < numOfPackets; i++)
		{
			//send porion of data
			char* tmp = PullPart(data, cursor, packetSize);
			SendDataToAll(tmp, packetSize);
			cursor += packetSize;
			printf("Send Progress: %%%4.2f\n", ((double)cursor / (double)size) * 100);
		}
		if (lastPackSize > 0) {
			//if extra info send here
			char* tmp = PullPart(data, cursor, lastPackSize);
			SendDataToAll(tmp, lastPackSize);
			cursor += lastPackSize;
			printf("Send Progress: %%%4.2f\n", ((double)cursor / (double)size) * 100);
		}
		return true;
	}

	bool SendFileToAll(std::string path, int PackSize) {
		std::ifstream in(path, std::ios::binary);
		if (in.is_open()) {

			in.seekg(0, in.end);
			int length = in.tellg();
			in.seekg(0, in.beg);

			if (length == 0) {
				printf("empty file");
				in.close();
				return false;
			}

			int numOfPackets = length / PackSize;
			int lastPackSize = length % PackSize;


			SendDataTToAll<int>(length);
			SendDataTToAll<int>(PackSize);
			SendDataTToAll<int>(numOfPackets);
			SendDataTToAll<int>(lastPackSize);

			int bytesSent = 0;

			char* packet;
			packet = new char[PackSize];
			for (int i = 0; i < numOfPackets; i++)
			{
				in.read(packet, PackSize);
				SendDataToAll(packet, PackSize);
				bytesSent += PackSize;
				printf("Send Progress: %%%4.2f\n", ((double)bytesSent / (double)length) * 100);
			}
			if (lastPackSize > 0) {
				in.read(packet, PackSize);
				SendDataToAll(packet, lastPackSize);
				bytesSent += lastPackSize;
				printf("Send Progress: %%%4.2f\n", ((double)bytesSent / (double)length) * 100);
			}
			return true;
		}
		return false;
	}

	bool SendTypeListToAll() {
		if (!SendDataTToAll<int>(DataTypes.size()))
			return false;
		for (std::vector<DataType>::iterator it = DataTypes.begin(); it != DataTypes.end(); ++it) {
			SendDataTToAll<DataType>(*it);
		}
		return true;
	}

	bool SendDataTypeToAll(DataType s) {
		if (DataTypes.TypeExists(s))
			return SendDataTToAll<int>(s.Identity);
	}

	bool SendDataTypeToAll(std::string s) {
		if (DataTypes.TypeExists(s))
			return SendDataTToAll<int>(DataTypes.FindType(s).Identity);
	}

	bool SendDataTypeToAll(int s) {
		if (DataTypes.TypeExists(s))
			return SendDataTToAll<int>(DataTypes.FindType(s).Identity);
	}

	bool SendDataTypeToAll(std::string s, int identity) {
		if (DataTypes.TypeExists(DataType(s, identity)))
			return SendDataTToAll<int>(DataTypes.FindType(DataType(s, identity)).Identity);
	}

	//bool SendCVMatToAll(cv::Mat img) {
	//	if (SendDataTToAll<int>(img.rows)) {
	//		if (SendDataTToAll<int>(img.cols)) {

	//			char* k = (char*)img.data;

	//			if (SendDataToAll(k, img.total() * img.elemSize())) {

	//				if (SendDataTToAll<int>(img.type())) {

	//					return true;
	//				}
	//			}
	//		}
	//	}
	//	return false;
	//}

	//send recv to from all end

	void AddDataType(std::string name, int identity) {
		DataTypes.AddType(name,identity);
	}

	void AddDataType(DataType dt) {
		DataTypes.AddType(dt);
	}

	void RemoveDataType(std::string name, int identity) {
		DataTypes.RemoveType(name, identity);
	}

	void RemoveDataType(DataType dt) {
		DataTypes.RemoveType(dt);
	}

	//bool SendCVMat(cv::Mat img, NamedSOCKET * ns) {
	//	if (SendDataT<int>(img.rows,ns)) {
	//		if (SendDataT<int>(img.cols,ns)) {

	//			char* k = (char*)img.data;

	//				if (SendData(k, img.total() * img.elemSize(),ns)) {

	//					if (SendDataT<int>(img.type(),ns)) {

	//						return true;
	//					}
	//				}
	//		}
	//	}
	//	return false;
	//}

	//bool RecvCVMat(cv::Mat* img, NamedSOCKET * ns) {
	//	int rows;
	//	if (!RecvDataT<int>(&rows,ns)) {
	//		return false;
	//	}
	//	int cols;
	//	if (!RecvDataT<int>(&cols,ns)) {
	//		return false;
	//	}
	//	char* data = nullptr;

	//	if (!RecvData(&data,ns)) {
	//		return false;
	//	}

	//	int imgtype;
	//	if (!RecvDataT<int>(&imgtype,ns)) {
	//		return false;
	//	}

	//	*img = cv::Mat(rows, cols, imgtype, data);

	//	return true;
	//}

};

