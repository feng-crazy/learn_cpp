
#ifndef _SPI_LIB_HPP
#define _SPI_LIB_HPP
#endif 

#include <stdint.h>
#include <linux/spi/spidev.h>
#include <sys/time.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    uint8_t mode;
    uint8_t bits_per_word;
    uint32_t speed;
    uint16_t delay;
} spi_config_t;

#ifdef __cplusplus
}
#endif

//class SPI{
//public:
#ifdef __cplusplus
class SpiDevice
{
private:
	char *m_spidev;
	int m_spifd;
	spi_config_t m_spiconfig;
	bool m_open;
public:
	SpiDevice(const char * p_spidev);
	SpiDevice(const char * p_spidev, spi_config_t *p_spi_config);
	~SpiDevice();
	bool begin();
	bool end();
	int spi_read(uint8_t *p_rxbuffer,int p_rxlen);
	int spi_write(uint8_t *p_txbuffer,int p_txlen);
	int xfer(uint8_t *p_txbuffer, int p_txlen, uint8_t *p_rxbuffer, int p_rxlen);
	bool setSpeed(uint32_t p_speed);
	bool setMode(uint8_t p_mode);
	bool setBitPerWord(uint8_t p_bit);
	bool setConfig(spi_config_t *p_spi_config);

};

#endif





