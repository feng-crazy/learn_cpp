#include "LT.Interaction.pb.h"

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

void ListMsg(const LT::interaction & msg)
{
 
	LT::interaction::BaseStationInfo base_info_msg =  msg.base_station_info();
	cout << base_info_msg.hostname() << endl;
	cout << base_info_msg.bs_id() << endl;
	cout << base_info_msg.ingroup_addr() <<endl<<endl;

	LT::interaction::TerminalInfo term_info_msg =  msg.terminal_info();
	cout << term_info_msg.hostname() << endl;
	cout << term_info_msg.ip_addr() << endl;
	cout << term_info_msg.mac_addr() <<endl<<endl;

	for(int i =0 ;i<msg.base_station_info_list_size();i++)
	{
		base_info_msg = msg.base_station_info_list(i);
		cout << base_info_msg.hostname() << endl;
		cout << base_info_msg.bs_id() << endl;
		cout << base_info_msg.ingroup_addr() <<endl<<endl;
	}

	for(int i =0 ;i<msg.terminal_info_list_size();i++)
	{
		term_info_msg = msg.terminal_info_list(i);
		cout << term_info_msg.hostname() << endl;
		cout << term_info_msg.ip_addr() << endl;
		cout << term_info_msg.mac_addr() <<endl<<endl;
	}

	LT::interaction::TermCommandInfo term_cmd_info = msg.terminal_cmd_info();
	cout << term_cmd_info.cmd_id() << endl;
	cout << term_cmd_info.cmd_arg1() <<endl<<endl;

}

int main(int argc, char* argv[])
{

	LT::interaction msg1;
	{
		fstream input("./log", ios::in | ios::binary);
		if (!msg1.ParseFromIstream(&input))
		{
			cerr << "Failed to parse address book." << endl;
			return -1;
		}
	}

	ListMsg(msg1);

}
