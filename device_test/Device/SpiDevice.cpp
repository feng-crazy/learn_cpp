
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>

#include <linux/spi/spidev.h>

#include "SpiDevice.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

bool SpiDevice::setBitPerWord(uint8_t p_bit)
{
	/* Set bits per word*/
	if (ioctl(m_spifd, SPI_IOC_WR_BITS_PER_WORD, &p_bit) < 0)
	{
		return false;
	}
	if (ioctl(m_spifd, SPI_IOC_RD_BITS_PER_WORD, &p_bit) < 0)
	{
		return false;
	}

	m_spiconfig.bits_per_word = p_bit;
	return true;
   
}
bool SpiDevice::setSpeed(uint32_t p_speed)
{
	/* Set SPI speed*/
	if (ioctl(m_spifd, SPI_IOC_WR_MAX_SPEED_HZ, &p_speed) < 0) {
		return false;
	}
	if (ioctl(m_spifd, SPI_IOC_RD_MAX_SPEED_HZ, &p_speed) < 0) {
		return false;
	}

	m_spiconfig.mode = p_speed;

	return true;
}

bool SpiDevice::setMode(uint8_t p_mode)
{
    /* Set SPI_POL and SPI_PHA */
    if (ioctl(m_spifd, SPI_IOC_WR_MODE, &p_mode) < 0)
    {
        return false;
    }
    if (ioctl(m_spifd, SPI_IOC_RD_MODE, &p_mode) < 0)
    {
        return false;
    }

    m_spiconfig.mode = p_mode;
    return true;
}

int SpiDevice::xfer(uint8_t *p_txbuffer, int p_txlen, uint8_t *p_rxbuffer, int p_rxlen)
{
    struct spi_ioc_transfer spi_message[2];
    memset(spi_message, 0, sizeof(spi_message));
    
    spi_message[0].tx_buf = (unsigned long)p_txbuffer;
    spi_message[0].len = p_txlen;
    spi_message[1].rx_buf = (unsigned long)p_rxbuffer;
    spi_message[1].len = p_rxlen;
    return ioctl(m_spifd, SPI_IOC_MESSAGE(2), spi_message);
}

int SpiDevice::write(uint8_t *p_txbuffer,int p_txlen)
{
    struct spi_ioc_transfer spi_message[1];
    memset(spi_message, 0, sizeof(spi_message));
    spi_message[0].tx_buf = (unsigned long)p_txbuffer;
    spi_message[0].rx_buf = 0;
    spi_message[0].len = p_txlen;
    spi_message[0].speed_hz =  m_spiconfig.speed;
    spi_message[0].bits_per_word = m_spiconfig.bits_per_word;

    int ret = ioctl(m_spifd, SPI_IOC_MESSAGE(1), spi_message);
    return ret;
}



int SpiDevice::read(uint8_t *p_rxbuffer,int p_rxlen)
{
    struct spi_ioc_transfer spi_message[1];
    memset(spi_message, 0, sizeof(spi_message));
    spi_message[0].tx_buf = 0;
    spi_message[0].rx_buf = (unsigned long)p_rxbuffer;
    spi_message[0].len = p_rxlen;
    spi_message[0].speed_hz =  m_spiconfig.speed;
    spi_message[0].bits_per_word = m_spiconfig.bits_per_word;

    int ret = ioctl(m_spifd, SPI_IOC_MESSAGE(1), spi_message);
    return ret;
}

bool SpiDevice::begin()
{
    /* open spidev device */
    if (m_open == true )
       return true;
    if (m_spidev == NULL)
       return false;
    m_spifd = open(m_spidev, O_RDWR);
  
    if (m_spifd < 0)
    {
        return false;
    }
    /* Set SPI_POL and SPI_PHA */
    if (ioctl(m_spifd, SPI_IOC_WR_MODE, &m_spiconfig.mode) < 0)
    {
        close(m_spifd);
        return false;
    }
    if (ioctl(m_spifd, SPI_IOC_RD_MODE, &m_spiconfig.mode) < 0)
    {
        close(m_spifd);
        return false;
    }

    /* Set bits per word*/
    if (ioctl(m_spifd, SPI_IOC_WR_BITS_PER_WORD, &m_spiconfig.bits_per_word) < 0)
    {
        close(m_spifd);
        return false;
    }
    if (ioctl(m_spifd, SPI_IOC_RD_BITS_PER_WORD, &m_spiconfig.bits_per_word) < 0)
    {
        close(m_spifd);
        return false;
    }

    /* Set SPI speed*/
    if (ioctl(m_spifd, SPI_IOC_WR_MAX_SPEED_HZ, &m_spiconfig.speed) < 0)
    {
        close(m_spifd);
        return false;
    }
    if (ioctl(m_spifd, SPI_IOC_RD_MAX_SPEED_HZ, &m_spiconfig.speed) < 0)
    {
        close(m_spifd);
        return false;
    }

    m_open = true;

    return true;

  
}
SpiDevice::SpiDevice(const char * p_spidev):m_spifd(-1)
{
  m_spidev = NULL;
  if (p_spidev != NULL ){
      m_spidev = (char *)malloc(strlen(p_spidev)+1);
      if (m_spidev != NULL) 
         strcpy(m_spidev,p_spidev);
  }
   m_open = false;

}
SpiDevice::SpiDevice(const char * p_spidev, spi_config_t *p_spi_config):m_spifd(-1)
{
  m_spidev = NULL;
  if (p_spidev != NULL )
  {
      m_spidev = (char *)malloc(strlen(p_spidev)+1);
      if (m_spidev != NULL) 
         strcpy(m_spidev,p_spidev);
  }
  if (p_spi_config != NULL)
  {
	memcpy(&m_spiconfig,p_spi_config,sizeof(spi_config_t));
  }
  else
  {
	m_spiconfig.mode = 0;
	m_spiconfig.speed = 1000000;
	m_spiconfig.bits_per_word = 8;
	m_spiconfig.delay = 0;
		
  }
 m_open = false;
}

SpiDevice::~SpiDevice()
{
	if (m_spidev != NULL )
	{
		free(m_spidev);
		m_spidev = NULL;
	}
	if (m_open)
		close(m_spifd);
}


bool SpiDevice::setConfig(spi_config_t *p_spi_config)
{
	if (p_spi_config != NULL)
	{
		memcpy(&m_spiconfig,p_spi_config,sizeof(spi_config_t));
		if (m_open)
		{
			/* Set SPI_POL and SPI_PHA */
			if (ioctl(m_spifd, SPI_IOC_WR_MODE, &m_spiconfig.mode) < 0)
			{
			  close(m_spifd);
			  return false;
			}
			if (ioctl(m_spifd, SPI_IOC_RD_MODE, &m_spiconfig.mode) < 0)
			{
			 close(m_spifd);
			 return false;
			}

			/* Set bits per word*/
			if (ioctl(m_spifd, SPI_IOC_WR_BITS_PER_WORD, &m_spiconfig.bits_per_word) < 0)
			{
			 close(m_spifd);
			 return false;
			}
			if (ioctl(m_spifd, SPI_IOC_RD_BITS_PER_WORD, &m_spiconfig.bits_per_word) < 0)
			{
			 close(m_spifd);
			 return false;
			}

			/* Set SPI speed*/
			if (ioctl(m_spifd, SPI_IOC_WR_MAX_SPEED_HZ, &m_spiconfig.speed) < 0) {
			 close(m_spifd);
			 return false;
			}
			if (ioctl(m_spifd, SPI_IOC_RD_MAX_SPEED_HZ, &m_spiconfig.speed) < 0) {
			 close(m_spifd);
			 return false;
			}
			return true;
		}
	}
	return false;
}



