#!/bin/sh

set -x 

gcc udpClient.c -o udpClient

gcc udpServer.c -o udpServer
