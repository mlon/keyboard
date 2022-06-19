#include "spi.h"

void spi_enable(uint8_t cs) { PORTB &= ~(1 << cs); }

void spi_disable(uint8_t cs) { PORTB |= (1 << cs); }

uint8_t spi_sendrecv(uint8_t data) {
  SPDR = data;

  while (!(SPSR & (1 << SPIF))) {
  };

  return SPDR;
}

void spi_send(uint8_t data) { spi_sendrecv(data); }
uint8_t spi_recv(void) { return spi_sendrecv(0xFF); }