/*
 * SerialDev.cpp
 *
 *  Created on: Feb 6, 2018
 *      Author: hdf_123
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <wiringPi.h>
#include "SerialDev.h"

#define RS485_PIN 8

SerialDev::SerialDev(const char *device, const int baud):_device(device),_baud(baud),_fd(-1)
{
//	_fd = serialOpen(_device,_baud);

}

SerialDev::~SerialDev()
{
	serialClose();
}




int SerialDev::serialOpen ()
{
	struct termios options ;
	speed_t myBaud ;
	int     status, fd ;

	switch (_baud)
	{
		case      50:	myBaud =      B50 ; break ;
		case      75:	myBaud =      B75 ; break ;
		case     110:	myBaud =     B110 ; break ;
		case     134:	myBaud =     B134 ; break ;
		case     150:	myBaud =     B150 ; break ;
		case     200:	myBaud =     B200 ; break ;
		case     300:	myBaud =     B300 ; break ;
		case     600:	myBaud =     B600 ; break ;
		case    1200:	myBaud =    B1200 ; break ;
		case    1800:	myBaud =    B1800 ; break ;
		case    2400:	myBaud =    B2400 ; break ;
		case    4800:	myBaud =    B4800 ; break ;
		case    9600:	myBaud =    B9600 ; break ;
		case   19200:	myBaud =   B19200 ; break ;
		case   38400:	myBaud =   B38400 ; break ;
		case   57600:	myBaud =   B57600 ; break ;
		case  115200:	myBaud =  B115200 ; break ;
		case  230400:	myBaud =  B230400 ; break ;
		case  460800:	myBaud =  B460800 ; break ;
		case  500000:	myBaud =  B500000 ; break ;
		case  576000:	myBaud =  B576000 ; break ;
		case  921600:	myBaud =  B921600 ; break ;
		case 1000000:	myBaud = B1000000 ; break ;
		case 1152000:	myBaud = B1152000 ; break ;
		case 1500000:	myBaud = B1500000 ; break ;
		case 2000000:	myBaud = B2000000 ; break ;
		case 2500000:	myBaud = B2500000 ; break ;
		case 3000000:	myBaud = B3000000 ; break ;
		case 3500000:	myBaud = B3500000 ; break ;
		case 4000000:	myBaud = B4000000 ; break ;

		default:
			return -2 ;
	}

	//使用非阻塞模式
	if ((fd = open (_device, O_RDWR | O_NOCTTY | O_NDELAY | O_NONBLOCK)) == -1)
		return -1 ;

//	if ((fd = open (_device, O_RDWR | O_NOCTTY | O_NDELAY )) == -1)
//		return -1 ;

	fcntl (fd, F_SETFL, O_RDWR) ;

	// Get and modify current options:

	tcgetattr (fd, &options) ;

	cfmakeraw   (&options) ;
	cfsetispeed (&options, myBaud) ;
	cfsetospeed (&options, myBaud) ;

	options.c_cflag |= (CLOCAL | CREAD) ;
	options.c_cflag &= ~PARENB ;
	options.c_cflag &= ~CSTOPB ;
	options.c_cflag &= ~CSIZE ;
	options.c_cflag |= CS8 ;
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG) ;
	options.c_oflag &= ~OPOST ;

	options.c_cc [VMIN]  =   0 ;
	options.c_cc [VTIME] = 100 ;	// Ten seconds (100 deciseconds)

	int re = tcsetattr (fd, TCSANOW, &options) ;
	if(re != 0)
	{
		printf("_setting_fd error, and errno is %d\n", (int)strerror(errno));
		close(fd);
		return -1;
	}

	ioctl (fd, TIOCMGET, &status);

	status |= TIOCM_DTR ;
	status |= TIOCM_RTS ;

	ioctl (fd, TIOCMSET, &status);

	usleep (10000) ;	// 10mS

	//因为rs232转rs485所以要设置这个引脚
    if(-1==wiringPiSetup())
    {
           printf("wiringPiSetup error\n");
           return -1;
    }
	pinMode(RS485_PIN,OUTPUT);
	digitalWrite(RS485_PIN,LOW);

	_fd = fd;
	return fd ;
}



void SerialDev::serialFlush ()
{
	tcflush (_fd, TCIOFLUSH) ;
}



void SerialDev::serialClose ()
{
	close (_fd) ;
}



void SerialDev::serialPutchar (const unsigned char c)
{
	write (_fd, &c, 1) ;
}



void SerialDev::serialPuts (const char *s)
{
	write (_fd, s, strlen (s)) ;
}


void SerialDev::serialPrintf (const char *message, ...)
{
	va_list argp ;
	char buffer [1024] ;

	va_start (argp, message) ;
	vsnprintf (buffer, 1023, message, argp) ;
	va_end (argp) ;

	serialPuts (buffer) ;
}



int SerialDev::serialDataAvail ()
{
	int result ;

	if (ioctl (_fd, FIONREAD, &result) == -1)
		return -1 ;

	return result ;
}



int SerialDev::serialGetchar ()
{
	uint8_t x ;
	if (read (_fd, &x, 1) != 1)
		return -1 ;

	return ((int)x) & 0xFF ;
}

int SerialDev::serialWrite(const unsigned char buff[], unsigned int len)
{
	if((_fd < 0) || (buff == NULL))
	{
		return 0;
	}

	// 确保数据能够全部写往端口。
	const unsigned char *current = buff;
	int remain = len;
	int ret = -1;
	digitalWrite(RS485_PIN,HIGH);
//	printf("digitalWrite(RS485_PIN,LOW) :%d \n",RS485_PIN);

	ret = write(_fd, current, remain);
//	for(int i =0;i<len;i++)
//	{
//		printf("%02x ",current[i]);
//	}

	int byte_time = 10000000/_baud + 1;
	int sleep_time = len * byte_time;
//	printf("baud :%d,byte_time:%d sleep_time:%d\n",_baud,byte_time,sleep_time);
	usleep (sleep_time) ;	// 10mS

	digitalWrite(RS485_PIN,LOW);
//	printf("\ndigitalWrite(RS485_PIN,HIGH);  ret:%d\n",ret);
	return ret;
}


int SerialDev::serialRead(unsigned char buff[], unsigned len)
{
	int ret_len = read(_fd, buff, len);
	return ret_len;
}

