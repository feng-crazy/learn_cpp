/*
 * UdpDevice.cpp
 *
 *  Created on: Sep 4, 2018
 *      Author: hdf_123
 */

#include "UdpDevice.h"

UdpDevice::UdpDevice(std::shared_ptr<spdlog::logger> logger):logger_(logger)
{
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(BROADCAST_LISTEN_PORT);
    broadcastSocket_ = -1;
}


bool UdpDevice::begin()
{
    int rc;
    int opt_val = 1;

    broadcastSocket_ = socket(AF_INET, SOCK_DGRAM, 0);
    if (broadcastSocket_ < 0) {
        logger_->critical("{}:UdpDevice cannot open broadcast socket; errno = {}.", broadcastSocket_, errno);
        return false;
    }

    if (setsockopt(broadcastSocket_, SOL_SOCKET, SO_BROADCAST, &opt_val, sizeof(opt_val)) < 0) {
        logger_->critical("{}:UdpDevice setsockopt (SO_BROADCAST) error; errno = {}.", broadcastSocket_, errno);
        return false;
    }

    if (setsockopt(broadcastSocket_, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val)) < 0){
        logger_->critical("{}:UdpDevice set reuse port error;  errno = {}.", broadcastSocket_, errno);
        return false;
    }

    rc = bind(broadcastSocket_, (struct sockaddr *) &servAddr, sizeof(servAddr));

    if (rc < 0) {
        logger_->critical("errno = {}: cannot bind port number {}.", errno, BROADCAST_LISTEN_PORT);
        return false;
    }

//    rc = fcntl(broadcastSocket_, F_GETFL, 0);
//    if (rc < 0) {
//        logger_->error("fcntl F_GETFL failed.");
//        return false;
//    }
//
//    //set non-blocking
//    if (fcntl(broadcastSocket_, F_SETFL, rc | O_NONBLOCK) < 0) {
//        logger_->error("fcntl F_GETFL failed.");
//        return false;
//    }


	return true;
}

int UdpDevice::read(void* buffer, size_t read_len)
{
    struct sockaddr_in cliAddr;
    socklen_t cliLen = sizeof(cliAddr);

    if (broadcastSocket_ < 0)
        return -1;

    int recv_len = recvfrom(broadcastSocket_, buffer, read_len, 0, (struct sockaddr *) &cliAddr, &cliLen);
    if (recv_len < 0) {
        if(errno == EWOULDBLOCK) {  //no messages pending
            return recv_len;
        }
        else {
            logger_->warn("UdpDevice errno = {}: the broadcast socket failed receiving data.", errno);
            return recv_len;
        }
    }
    else if(recv_len==0)
    {
    	return recv_len;
    }

    return recv_len;
}

UdpDevice::~UdpDevice()
{
    if (broadcastSocket_ > 0)
        close(broadcastSocket_);
}

