/*
 * spi_stl_test.cpp
 *
 *  Created on: Dec 18, 2017
 *      Author: hdf_123
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <linux/spi/spidev.h>
#include "../Device/SpiDevice.h"

#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <time.h>
#include <math.h>
#include <chrono>
#include <vector>

using namespace std;
using std::vector;

int main()
{
	spi_config_t spi_config;
	spi_config.mode = SPI_CPHA | SPI_CPOL;
	spi_config.speed = 2000000;
	spi_config.delay = 0;
	spi_config.bits_per_word = 8;
	SpiDevice *spi_dev = new SpiDevice("/dev/spidev0.0",&spi_config);

	if(!spi_dev->begin())
	{
		printf("spi init failed\n");
		return 0;
	}

	while(true)
	{
		vector<unsigned char>vec_buff(2048);
		unsigned char buff[2048] = {0x00};
		unsigned char *p = vec_buff.data();

		int spi_read_len = -1;
		spi_read_len = spi_dev->spi_read(p, sizeof(buff));
		printf("spi read-------len = %d------------\n",spi_read_len);
		for(int i=0;i<spi_read_len;i++)
		{
			printf("%02x ",vec_buff[i]);
		}
		printf("\n");
	}
}



