#pragma once

#include <stdint.h>

// 50kHz clock
// baud rate (clock) = 4e6 / (2*(SPI1BRG + 1))
#define SPI_BRG_VALUE (3)
//#define SPI_BRG_VALUE (500)

void init_spi();

// read `bytes` bytes from `address` into `out_buffer`.
// `bytes` must be a multiple of the SPI hardware FIFO length, 32
void read_spi_flash(uint32_t address, uint16_t bytes, uint8_t* out_buffer);