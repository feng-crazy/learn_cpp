/*
 *
 * Copyright (c) 2014 Philippe Van Hecke <lemouchon@gmail.com>
 *
 * you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#ifndef _SPI_LIB_HPP
#define _SPI_LIB_HPP
#endif 

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
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
class SpiDevice{
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





