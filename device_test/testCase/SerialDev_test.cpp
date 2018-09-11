/*
 * SerialDev_test.cpp
 *
 *  Created on: Feb 6, 2018
 *      Author: hdf_123
 */
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <linux/spi/spidev.h>

#include "../Device/SerialDev.h"

int main()
{
	SerialDev serial_dev("/dev/ttyAMA0", 115200);

	if(serial_dev.serialOpen()<0)
	{
		printf("open serial failure\n");
		return -1;
	}

	unsigned char buff[4] = {0xFF,0xF8,0xF4,0xF2};
	unsigned char read_buff[1024];
	for (;;)
	{
		int write_ret = serial_dev.serialWrite(buff,sizeof(buff));
		printf("write success ret len = %d\n",write_ret);

		int retval;
		struct timeval tv;
		fd_set rfds;

	    int fd = serial_dev.serialGetFd();
	    printf("fd:%d\n",fd);
	    tv.tv_sec = 0;
	    tv.tv_usec = 1000;

		FD_ZERO(&rfds);
		FD_SET( fd,&rfds );
		retval = select(fd+1,&rfds,NULL,NULL,&tv);
		if( retval < 0 )
		{
			printf("error\r\n");
			break;
		}
		else if(retval)
		{
			int read_len = serial_dev.serialRead(read_buff,1024);
			printf("read_len = %d\n",read_len);
			for(int i=0;i<read_len;i++)
			{
				printf("%02X ",read_buff[i]);
			}
			printf("\n");
		}
		else
		{

		}

		sleep(1);
	}
	return 0;
}
