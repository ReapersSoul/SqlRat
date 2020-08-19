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
#include <memory>

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
		connected = false;
	}

	NamedSOCKET(SOCKET s, std::string Name) {
		sock = s;
		name = Name;
	}
	bool connected;
	SOCKET sock;
	std::string name;
};

class Single_NetworkBase
{
protected:

	std::string Name;

	WSADATA wsaData;
	int iResult;

	struct addrinfo* result = NULL, * ptr = NULL, hints;

	SOCKET ListenSocket = INVALID_SOCKET;

	DataTypeList DataTypes;

	std::string ip;

	std::string port;


	NamedSOCKET ConnectedSocket;

public:


	void SetName(std::string s) {
		Name = s;
	}

	std::string getIP() {
		return ip;
	}

	std::string getPort() {
		return port;
	}

	std::shared_ptr<char> addToEnd(std::shared_ptr<char> first, int firstsize, std::shared_ptr<char> second, int secondsize) {
		std::shared_ptr<char> ret;
		ret.reset(new char[firstsize + secondsize]);
		for (int i = 0; i < firstsize; i++)
		{
			ret.get()[i] = first.get()[i];
		}
		for (int i = 0; i < secondsize; i++)
		{
			ret.get()[firstsize + i] = second.get()[i];
		}
		return ret;
	}

	std::shared_ptr<char> addNullToEnd(std::shared_ptr<char> src, int size) {
		std::shared_ptr<char> ret = src;

		ret.get()[size] = (char)'\0';

		return ret;
	}

	std::shared_ptr<char> PullPart(std::shared_ptr<char> src, int start, int bytesToPull) {
		std::shared_ptr<char> ret;
		ret.reset(new char[bytesToPull]);
		for (int i = 0; i < bytesToPull; i++)
		{
			ret.get()[i] = src.get()[i + start];
		}
		return ret;
	}


	bool SendData(char* data, int size)
	{
		//send size
		iResult = send(ConnectedSocket.sock, reinterpret_cast<char*>(&size), sizeof(int), 0);
		if (iResult == SOCKET_ERROR) {
			//printf("send failed: %d\n", WSAGetLastError());
			return false;
		}

		//send data
		iResult = send(ConnectedSocket.sock, data, size, 0);
		if (iResult == SOCKET_ERROR) {
			//printf("send failed: %d\n", WSAGetLastError());
			return false;
		}
		Sleep(100);
		return true;
	}

	template<typename T>
	bool SendDataT(T t) {
		return SendData(reinterpret_cast<char*>(&t), sizeof(T));
	}

	template<>
	bool SendDataT<std::string>(std::string s) {
		return SendData((char*)s.c_str(), s.size() + 1);
	}

	template<>
	bool SendDataT<std::wstring>(std::wstring s) {
		return SendData((char*)s.c_str(), s.capacity()*sizeof(wchar_t));
	}


	bool RecvData(std::shared_ptr<char>* data)
	{


		int size = 0;

		iResult = recv(ConnectedSocket.sock, reinterpret_cast<char*>(&size), sizeof(int), 0);
		if (iResult > 0) {
			//printf("Bytes received: %d\n", iResult);
			//std::cout << "size of packet: " << size << std::endl;
		}
		else if (iResult == 0) {
			printf("Connection closed\n");
			ConnectedSocket.connected = false;
			return false;
		}
		else {
			//printf("recv failed: %d\n", WSAGetLastError());
			return false;
		}

		std::shared_ptr<char> ret(new char[size]);

		iResult = recv(ConnectedSocket.sock, ret.get(), size, 0);
		if (iResult > 0) {
			//printf("Bytes received: %d\n", iResult);
		}
		else if (iResult == 0) {
			printf("Connection closed\n");
			ConnectedSocket.connected = false;
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
	bool RecvDataT(T * item)
	{
		std::shared_ptr<char> data;

		if (!RecvData(&data)) {
			return NULL;
		}

		std::shared_ptr < T> ret(reinterpret_cast<T*>(data.get()));

		*item = *ret;
	}

	template<>
	bool RecvDataT<std::string>(std::string * s)
	{
		std::shared_ptr<char> data;

		if (RecvData(&data)) {

			*s = data.get();
			return true;
		}
		return false;
	}

	template<>
	bool RecvDataT<std::wstring>(std::wstring* s)
	{
		std::shared_ptr<char> data;

		if (RecvData(&data)) {

			*s = (wchar_t *)data.get();
			return true;
		}
		return false;
	}

	bool SendBatchData(std::shared_ptr<char> data, int size, int packetSize) {
		int numOfPackets = size / packetSize;
		int lastPackSize = size % packetSize;


		SendDataT<int>(size);
		SendDataT<int>(numOfPackets);
		SendDataT<int>(packetSize);
		SendDataT<int>(lastPackSize);

		int cursor = 0;
		for (int i = 0; i < numOfPackets; i++)
		{
			//send porion of data
			std::shared_ptr<char> tmp = PullPart(data, cursor, packetSize);
			SendData(tmp.get(), packetSize);
			cursor += packetSize;
			printf("Send Progress: %%%4.2f\n", ((double)cursor / (double)size) * 100);
		}
		if (lastPackSize > 0) {
			//if extra info send here
			std::shared_ptr<char> tmp = PullPart(data, cursor, lastPackSize);
			SendData(tmp.get(), lastPackSize);
			cursor += lastPackSize;
			printf("Send Progress: %%%4.2f\n", ((double)cursor / (double)size) * 100);
		}
		return true;
	}

	bool RecvBatchData(std::shared_ptr<char> * data) {

		int size;
		if (!RecvDataT<int>(&size)) {
			return false;
		}
		int numOfPackets;
		if (!RecvDataT<int>(&numOfPackets)) {
			return false;
		}
		int packetSize;
		if (!RecvDataT<int>(&packetSize)) {
			return false;
		}
		int lastPackSize;
		if (!RecvDataT<int>(&lastPackSize)) {
			return false;
		}

		std::shared_ptr<char> ret;
		ret.reset( new char[(numOfPackets * packetSize) + lastPackSize]);

		int currentSize = 0;

		for (int i = 0; i < numOfPackets; i++)
		{
			//recv portion of data
			std::shared_ptr<char> tmp;
			if (RecvData(&tmp)) {
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
			std::shared_ptr<char> tmp;
			tmp.reset( new char[lastPackSize]);
			if (RecvData(&tmp)) {
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

	bool SendFile(std::string path, int PackSize) {
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


			SendDataT<int>(length);
			SendDataT<int>(PackSize);
			SendDataT<int>(numOfPackets);
			SendDataT<int>(lastPackSize);

			int bytesSent = 0;

			std::shared_ptr<char> packet;
			packet.reset( new char[PackSize]);
			for (int i = 0; i < numOfPackets; i++)
			{
				in.read(packet.get(), PackSize);
				SendData(packet.get(), PackSize);
				bytesSent += PackSize;
				printf("Send Progress: %%%4.2f\n", ((double)bytesSent / (double)length) * 100);
			}
			if (lastPackSize > 0) {
				in.read(packet.get(), PackSize);
				SendData(packet.get(), lastPackSize);
				bytesSent += lastPackSize;
				printf("Send Progress: %%%4.2f\n", ((double)bytesSent / (double)length) * 100);
			}
			return true;
		}
		return false;
	}

	bool RecvFile(std::string path) {
		std::ofstream out(path, std::ios::binary);

		if (out.is_open() && out.good()) {
			int length;
			if (!RecvDataT<int>(&length)) {
				return false;
			}
			int PackSize;
			if (!RecvDataT<int>(&PackSize)) {
				return false;
			}
			int numOfPackets;
			if (!RecvDataT<int>(&numOfPackets)) {
				return false;
			}
			int lastPackSize;
			if (!RecvDataT<int>(&lastPackSize)) {
				return false;
			}

			int bytesRecived = 0;

			for (int i = 0; i < numOfPackets; i++)
			{
				std::shared_ptr<char> packet;
				packet.reset( new char[PackSize]);
				RecvData(&packet);
				out.write(packet.get(), PackSize);
				bytesRecived += PackSize;
				printf("Recive Progress: %%%4.2f\n", ((double)bytesRecived / (double)length) * 100);
			}
			if (lastPackSize > 0) {
				std::shared_ptr<char> packet;
				packet.reset( new char[lastPackSize]);
				RecvData(&packet);
				out.write(packet.get(), lastPackSize);
				bytesRecived += lastPackSize;
				printf("Recive Progress: %%%4.2f\n", ((double)bytesRecived / (double)length) * 100);
			}
			out.close();
			return true;
		}
		return false;
	}

	bool SendTypeList() {
		if(!SendDataT<int>(DataTypes.size()))
			return false;
		for (std::vector<DataType>::iterator it = DataTypes.begin(); it != DataTypes.end(); ++it) {
			SendDataT<DataType>(*it);
		}
		return true;
	}

	bool RecvTypeList() {
		int size;
		if (!RecvDataT<int>(&size)) {
			return false;
		}
		for (int i = 0; i < size; i++)
		{
			DataType dt;
			if (!RecvDataT<DataType>(&dt)) {
				return false;
			}

			DataTypes.AddType(dt);
		}
		return true;
	}

	bool SendDataType(DataType s) {
		if(DataTypes.TypeExists(s))
			return SendDataT<int>(s.Identity);
	}

	bool SendDataType(std::string s) {
		if (DataTypes.TypeExists(s))
			return SendDataT<int>(DataTypes.FindType(s).Identity);
	}

	bool SendDataType(int s) {
		if (DataTypes.TypeExists(s))
			return SendDataT<int>(DataTypes.FindType(s).Identity);
	}

	bool SendDataType(std::string s, int identity) {
		if (DataTypes.TypeExists(DataType(s,identity)))
			return SendDataT<int>(DataTypes.FindType(DataType(s, identity)).Identity);
	}

	bool RecvDataType(DataType * s) {
		int identity;
		RecvDataT<int>(&identity);
		if (DataTypes.TypeExists(identity)) {
			*s = DataTypes.FindType(identity);
			return true;
		}
		else {
			*s = DataTypes.FindType(0);
			return false;
		}
	}

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

	//bool SendCVMat(cv::Mat img) {
	//	if (SendDataT<int>(img.rows)) {
	//		if (SendDataT<int>(img.cols)) {

	//			std::shared_ptr<char> k( (char*)img.data);

	//				if (SendData(k, img.total() * img.elemSize())) {

	//					if (SendDataT<int>(img.type())) {

	//						return true;
	//					}
	//				}
	//		}
	//	}
	//	return false;
	//}

	//bool RecvCVMat(cv::Mat* img) {
	//	int rows;
	//	if (!RecvDataT<int>(&rows)) {
	//		return false;
	//	}
	//	int cols;
	//	if (!RecvDataT<int>(&cols)) {
	//		return false;
	//	}
	//	std::shared_ptr<char> data = nullptr;

	//	if (!RecvData(&data)) {
	//		return false;
	//	}

	//	int imgtype;
	//	if (!RecvDataT<int>(&imgtype)) {
	//		return false;
	//	}

	//	*img = cv::Mat(rows, cols, imgtype, data);

	//	return true;
	//}

};

