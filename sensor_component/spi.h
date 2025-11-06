#ifndef SPI_H
#define SPI_H

#include <stdint.h>

void spi_init(void);
void spi_write(uint8_t addr, uint8_t tx_data);

uint8_t spi_read( uint8_t addr);
#endif