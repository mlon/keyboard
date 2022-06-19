#include <avr/io.h>

#include "mcp23s18.h"
#include "spi.h"

#define MOSI PINB3
#define MISO PINB4
#define SCK PINB5
#define EXP_CS PINB1

int main(void) {
  // configure SPI

  DDRB |= (1 << MOSI) | (1 << SCK) | (1 << EXP_CS) | (1 << PINB2);
  SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);

  PORTB |= (1 << EXP_CS);

  // configure B ports of expander as input-pullup
  // it's already set to read by default so we only need to enable the pull-ups
  mcp23s18_write(EXP_CS, MCP23S18_GPPUB, 0xFF);

  // configure A ports of expander as output
  mcp23s18_write(EXP_CS, MCP23S18_IODIRA, 0xFF);

  while (1) {
    uint8_t input = mcp23s18_read(EXP_CS, MCP23S18_GPIOB);
    uint8_t connected = !(input & (1 << 0));

    mcp23s18_write(EXP_CS, MCP23S18_OLATA, connected << 7);
  }
}