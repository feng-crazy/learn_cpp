/*
 * generate_data.cpp
 *
 *  Created on: Dec 11, 2017
 *      Author: hdf_123
 */

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <execinfo.h>
#include <cxxabi.h>
#include <dlfcn.h>
#include <time.h>

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>


/******************************************************************************
作者: 何登锋, 2017.12.8
功能描述:
	填充发送数据包checksum位的值
参数说明:
	buff: 发送的数据包
	pack_length: 需要计算校验和的数据长度，从帧头到帧尾
返回值:  无
******************************************************************************/
static void fill_check_sum(unsigned char *buff,unsigned int&pack_length)
{
	unsigned short calc_check_sum = 0;
	for(unsigned int i=0;i<pack_length;i++)
	{
		calc_check_sum = calc_check_sum + buff[i];
	}
	buff[pack_length++] = (calc_check_sum>>8) & 0xFF;
	buff[pack_length++] = calc_check_sum & 0xFF;
}

int main()
{
	int fd = open("/dev/urandom", O_RDWR);
	if(fd<0)
	{
		printf("open dev failed\n");
	}
	unsigned short pack_len = 30;
	unsigned short seq = 0x8b6e;
	for(int i=0;i<10;i++)
	{
		unsigned char buff[30] = {0x00};
		unsigned int index = 0;
		buff[index++] = 0xf8;
		buff[index++] = 0xf4;
		buff[index++] = 0xf2;
		buff[index++] = 0x11;
		buff[index++] = (pack_len>>8) & 0xFF;
		buff[index++] = pack_len & 0xFF;
		buff[index++] = (seq>>8) & 0xFF;
		buff[index++] = seq & 0xFF;
		int ret = read(fd,&buff[index],14);
//		printf("ret =%d\n",ret);
		index = index + ret;

		buff[index++] = 0x00;
		buff[index++] = 0x1d;
		buff[index++] = 0x62;
		buff[index++] = 0xd8;

		buff[index++] = 0xa5;
		buff[index++] = 0x5a;
		fill_check_sum(buff,index);
		for(int k=0;k<pack_len;k++)
		{
			printf("%02x ",buff[k]);
		}
		printf("\n");
		seq++;
	}

	return 0;
}
