#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <spidev_lib++.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

spi_config_t spi_config;
uint8_t tx_buffer[9] = {0x55,0x5a,0x55,0x01,10,0,11,0xfe,0xff};
uint8_t rx_buffer[2000] = {0x00};


int  main( void)
{

  SpiDevice *mySPI = NULL;

  spi_config.mode=SPI_CPHA | SPI_CPOL;
  spi_config.speed=2000000;
  spi_config.delay=0;
  spi_config.bits_per_word=8;

  mySPI=new SpiDevice("/dev/spidev0.0",&spi_config);

  if (mySPI->begin())
  {
//    sprintf((char*)tx_buffer,"hello world");
//    printf("sending %s, to spidev2.0 in full duplex \n ",(char*)tx_buffer);
//    int recv_ret = mySPI->xfer(tx_buffer,sizeof(tx_buffer),rx_buffer,sizeof(rx_buffer));
//	printf("recv_ret = %d\n",recv_ret);
//	for (int ret = 0; ret < sizeof(rx_buffer); ret++) {
////		if(rx_buffer[ret] == 0)
////			printf("\t\n\n");
//		printf("%d ", rx_buffer[ret]);
//		if (rx_buffer[ret] == 199)
//			printf("\t\n\n");
//	}
//	printf("\n");


	int send_ret = mySPI->write(tx_buffer,ARRAY_SIZE(tx_buffer));
	printf("send_ret = %d read_len = %d\n",send_ret,ARRAY_SIZE(rx_buffer));
	int recv_ret = mySPI->read(rx_buffer,ARRAY_SIZE(rx_buffer));
	printf("recv_ret = %d\n",recv_ret);
	for (int ret = 0; ret < sizeof(rx_buffer); ret++) {
//		if(rx_buffer[ret] == 0)
//			printf("\t\n\n");
		printf("%d ", rx_buffer[ret]);
		if (rx_buffer[ret] == 199)
			printf("\t\n\n");
	}
	printf("\n");
    //mySPI->end();
    delete mySPI; 
  }
 return 1;
}
