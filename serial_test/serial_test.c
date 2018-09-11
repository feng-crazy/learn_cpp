#include <errno.h>
#include <string.h>
//#include <wiringpi.h>
#include <wiringserial.h>

int main(int argc, char *argv[])
{
	int fd;
	if ((fd = serialOpen("/dev/ttyAMA0", 115200)) < 0)
	{
		fprintf(stderr, "Unable to open serial device: %s\n", strerror(errno));
		return 1 ;
	}
	serialPuts(fd, "uart send test, just by launcher");
	
	for (;;)
	{
		if (serialDataAvail(fd) > 0)
		{
			putchar(serialGetchar(fd));	
		}
		
	}
	return 0;
}