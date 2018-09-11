#include "LT.Interaction.pb.h"

 
#include <string>
#include <iostream>
#include <fstream>


using namespace std;

int main(void)
{

	LT::interaction msg;
	msg.set_msg_id(1);
	LT::interaction::BaseStationInfo *base_info_msg;
	base_info_msg = msg.mutable_base_station_info();
	base_info_msg->set_hostname("base 0");
	base_info_msg->set_bs_id(1);
	base_info_msg->set_ingroup_addr(2);
//	msg1.set_allocated_base_station_info(base_info_msg);


	LT::interaction::TerminalInfo *term_info_msg;
	term_info_msg = msg.mutable_terminal_info();
	term_info_msg->set_hostname("term 0");
	term_info_msg->set_mac_addr("b1:b2:b3:b4:b5:b6");
	term_info_msg->set_ip_addr("192.168.0.112");
//	msg1.set_allocated_terminal_info(term_info_msg);


	base_info_msg = msg.add_base_station_info_list();
	base_info_msg->set_hostname("b1");
	base_info_msg->set_bs_id(1);
	base_info_msg->set_ingroup_addr(3);

	base_info_msg = msg.add_base_station_info_list();
	base_info_msg->set_hostname("b2");
	base_info_msg->set_bs_id(1);
	base_info_msg->set_ingroup_addr(4);

	cout<<msg.base_station_info_list_size()<<endl;

	term_info_msg = msg.add_terminal_info_list();
	term_info_msg->set_hostname("t1");
	term_info_msg->set_mac_addr("b1:b2:b3:b4:b5:b6");
	term_info_msg->set_ip_addr("192.168.0.113");

	term_info_msg = msg.add_terminal_info_list();
	term_info_msg->set_hostname("t2");
	term_info_msg->set_mac_addr("b1:b2:b3:b4:b5:b6");
	term_info_msg->set_ip_addr("192.168.0.114");

	cout<<msg.terminal_info_list_size()<<endl;

//	string out_str;
//	msg.SerializeToString(&out_str);
//	cout << out_str << endl;


	LT::interaction::TermCommandInfo *term_cmd_info = msg.mutable_terminal_cmd_info();
	LT::interaction::TermCommandInfo::TermCommandId term_cmd_id = term_cmd_info->ENTER_FIXED_CMD;
	term_cmd_info->set_cmd_id(term_cmd_id);
	term_cmd_info->set_cmd_arg1("term cmd arg 1");


	// Write the new address book back to disk.
	fstream output("./log", ios::out | ios::trunc | ios::binary);

	if (!msg.SerializeToOstream(&output))
	{
		cerr << "Failed to write msg." << endl;
		return -1;
	}
	return 0;
}
