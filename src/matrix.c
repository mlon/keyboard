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

unsigned char mkStates[8] = {0};
unsigned char brStates[8] = {0};
unsigned int times[8][5] = {0};

void pulseClock(unsigned char clockPin) {
  PORTD |= (1 << clockPin);
  _delay_ms(5);
  PORTD &= ~(1 << clockPin);
};

void readInput(unsigned char *mkInput, unsigned char *brInput) {
  unsigned int matrixInput = 0;
  // lower Latch pin to load data in
  PORTB |= (1 << CLK_INH);
  PORTD &= ~(1 << LAT_IN);
  _delay_ms(5);
  PORTD |= (1 << LAT_IN);
  PORTB &= ~(1 << CLK_INH);

  // shift data in
  for (unsigned char j = 0; j < 10; j++) {
    unsigned char val = 1 & (PIND >> SFT_IN);
    unsigned int bit = val << j;
    matrixInput |= bit;
    pulseClock(CLK_IN);
  }
  *brInput = matrixInput & 0b11111;
  *mkInput = (matrixInput >> 5) & 0b11111;
}

void initMatrix(void) {
  DDRD |= (1 << CLK_OUT) | (1 << CLR_OUT) | (1 << SFT_OUT) | (1 << CLK_IN) |
          (1 << LAT_IN);
  DDRB |= (1 << CLK_INH);

  // clear input shift register
  PORTD &= ~(1 << CLR_OUT);
  pulseClock(CLK_OUT);
  PORTD |= (1 << CLR_OUT);
}

void scanRow(unsigned char t, unsigned char mk, unsigned char br) {
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
};

void scanMatrix(void) {
  // queue up high into the shift register
  PORTD |= (1 << SFT_OUT);

  for (int t = 0; t < 8; t++) {
    // pulse the clock to load the next queued up value into the shift
    // register
    pulseClock(CLK_OUT);

    unsigned char mkInput = 0;
    unsigned char brInput = 0;
    readInput(&mkInput, &brInput);
    scanRow(t, mkInput, brInput);

    // queue up high into the shift register
    PORTD &= ~(1 << SFT_OUT);
  }
};