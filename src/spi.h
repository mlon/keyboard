#pragma once
#include <avr/io.h>

void spi_send(uint8_t data);
uint8_t spi_recv(void);
void spi_enable(uint8_t cs);
void spi_disable(uint8_t cs);