/*
 * check_data.cpp
 *
 *  Created on: Dec 21, 2017
 *      Author: hdf_123
 */

#include "LTCommon.h"
#include "LTConfig.h"
#include "LTUtilities.h"

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <time.h>
#include <math.h>
#include <chrono>

#include "DataDriver.h"
#include "DataThreads.h"
#include "Device/SpiDevice.h"


#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>

using std::vector;
using std::deque;
using std::string;
using std::shared_ptr;

using namespace LTUtilities;
using namespace std::chrono;


int main(int argc, char *argv[])
{
	string data_str;
	if (argc != 2) {
		//printf("Frame file name is required\n");
		//exit(0);
		data_str = "f8 f4 f2 11 00 1e 1f fc 01 32 fe f4 3d 5e fe 53 ff 93 ff d9 f5 79 00 07 2d 6d a5 5a 0e 83";
	}
	else
	{
		data_str = std::string(argv[1]);
	}
	printf("data_str:%s\n",data_str.c_str());

	unsigned char array[1024] = {0x00};

	vector<string> segs;
	std::stringstream ss(data_str);
	std::istream_iterator<string> is(ss);
	std::copy_if(is, std::istream_iterator<string>(), back_inserter(segs),
			[](string item)
			{
				return item.find_first_of(" .s\n\t\r") == string::npos;
			}
	);

	if(segs.empty())
	{
		printf("segs.empty()\n");
		return 0 ;
	}

	printf("seg --------------------size = %d\n",segs.size());
	for (size_t i = 0; i < segs.size(); i++)
	{
		printf("%s ",segs[i].c_str());
		array[i]= (unsigned char)(strtol(segs[i].c_str(), NULL, 16));
	}
	printf("\n\n");

	printf("array data---------------------------\n");
	for (size_t i = 0; i < segs.size(); i++)
	{
		printf("%02x ",array[i]);

	}
	printf("\n\n");

	Frame::FrameType ftype = Frame::Invalid;

	ftype = Frame::GetType(array, sizeof(array));

	if (ftype == Frame::Invalid)
	{
		printf("ftype is invalid \n");
		return 0;
	}

	unsigned short len = Frame::GetLength(array, sizeof(array));
	printf("len is : %d\n",len);

	unsigned short seq = (array[6] << 8) + array[7];
	printf("seq is : %d %02x %02x\n",seq,array[6],array[7]);

	if(!Frame::judge_check_sum(array,len))
	{
		printf("judge_check_sum is failed\n");
		return 0;
	}

	return 0;
}


