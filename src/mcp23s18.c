#include "mcp23s18.h"
#include "spi.h"

#define WRITE_OPCODE 0x40
#define READ_OPCODE 0x41

uint8_t mcp23s18_read(uint8_t cs, uint8_t addr) {
  spi_enable(cs);
  spi_send(READ_OPCODE);
  spi_send(addr);
  uint8_t ret = spi_recv();
  spi_disable(cs);
  return ret;
}

void mcp23s18_write(uint8_t cs, uint8_t addr, uint8_t value) {
  spi_enable(cs);
  spi_send(WRITE_OPCODE);
  spi_send(addr);
  spi_send(value);
  spi_disable(cs);
}