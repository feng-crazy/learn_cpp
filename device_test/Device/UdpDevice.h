/*
 * UdpDevice.h
 *
 *  Created on: Sep 4, 2018
 *      Author: hdf_123
 */

#ifndef LTHUB_LTHUB_DEVICE_UDPDEVICE_H_
#define LTHUB_LTHUB_DEVICE_UDPDEVICE_H_

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/utsname.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <net/if.h>
#include "spdlog/spdlog.h"


class UdpDevice {
public:
	UdpDevice(std::shared_ptr<spdlog::logger> logger);
	bool begin();
	int read(void *buffer, size_t read_len);

	virtual ~UdpDevice();

	const short BROADCAST_LISTEN_PORT = 56650;

	int broadcastSocket_;

private:
	struct sockaddr_in servAddr;
	std::shared_ptr<spdlog::logger> logger_;

};

#endif /* LTHUB_LTHUB_DEVICE_UDPDEVICE_H_ */
