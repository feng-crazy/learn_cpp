/*
 * SerialDev.h
 *
 *  Created on: Feb 6, 2018
 *      Author: hdf_123
 */

#ifndef LTHUB_LTHUB_DEVICE_SERIALDEV_H_
#define LTHUB_LTHUB_DEVICE_SERIALDEV_H_

class SerialDev {
public:
	SerialDev(const char *device, const int baud);
	virtual ~SerialDev();

	int serialOpen();
	void serialClose();
	void serialFlush();
	void serialPutchar(const unsigned char c);
	void serialPuts(const char *s);
	void serialPrintf(const char *message, ...);
	int serialDataAvail();
	int serialGetchar();
	int serialWrite(const unsigned char buff[], unsigned int len);
	int serialRead(unsigned char buff[], unsigned len);
	int serialGetFd(){return _fd;}
	void serialSetFd(int fd){_fd = fd;}

private:
	char *_device;
	int _baud;
	int _fd;
};

#endif /* LTHUB_LTHUB_DEVICE_SERIALDEV_H_ */
