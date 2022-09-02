#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#include "matrix.h"
#include "midi.h"
#include "millis.h"
#include "wiring.h"

#define setBit(p, m) ((p) |= _BV((m)))
#define clearBit(p, m) ((p) &= ~_BV((m)))
#define getBit(p, m) ((p)&_BV(m))

static unsigned char mkStates[8] = {0};
static unsigned char brStates[8] = {0};
static unsigned int times[8][5] = {0};

unsigned char spiReadWrite(unsigned char data) {
  SPDR = data;
  asm volatile("nop");
  while (!(SPSR & (1 << SPIF)))
    ;

  return SPDR;
}

unsigned int readRowInput(unsigned char t) {
  // latch inputs into shift register
  PORTB |= (1 << LATCH);

  // read first input shift register
  unsigned char inputLow = spiReadWrite(0);

  // read second shift register and write next output shift register
  unsigned char inputHi = spiReadWrite(1 << ((t + 1) % 8));

  // start loading inputs into registers again
  PORTB &= ~(1 << LATCH);

  return (inputHi << 8) | inputLow;
}

void initMatrix(void) {
  DDRB |= (1 << MOSI) | (1 << SCK) | (1 << LATCH);
  SPCR = (1 << SPE) | (1 << MSTR);
  SPSR = (1 << SPI2X);

  spiReadWrite(0b00000001);
}

void scanMatrix(void) {
  for (int t = 0; t < 8; t++) {
    unsigned int input = readRowInput(t);
    unsigned char mk = (input >> 5) & 0b11111;
    unsigned char br = input & 0b11111;

    for (unsigned char j = 0; j < 5; j++) {
      if (getBit(br, j)) {
        // break

        if (!getBit(brStates[t], j)) {
          setBit(brStates[t], j);
          times[t][j] = millis();
        }

        if (getBit(mk, j)) {
          // make
          if (!getBit(mkStates[t], j)) {
            sendKey(t, j, true, millis() - times[t][j]);
            setBit(mkStates[t], j);
          }
        } else {
          if (getBit(mkStates[t], j)) {
            clearBit(mkStates[t], j);
            times[t][j] = millis();
          }
        }
      } else {
        if (getBit(brStates[t], j)) {
          sendKey(t, j, false, millis() - times[t][j]);
          clearBit(brStates[t], j);
        }
      }
    }
  }
};