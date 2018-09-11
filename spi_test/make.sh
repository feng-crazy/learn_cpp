#!/bin/sh
set -x
arm-linux-gnueabihf-gcc -o spidev_test spidev_test.c
cp spidev_test /home/pi/nfs_share
