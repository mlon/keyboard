#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>

#include "matrix.h"

// PortB
#define CLK_INH 0

#define CLK_IN 2
#define LAT_IN 3
#define SFT_IN 4
#define CLK_OUT 5
#define SFT_OUT 6
#define CLR_OUT 7

void pulseClock(unsigned char clockPin) {
  PORTD |= (1 << clockPin);
  _delay_ms(5);
  PORTD &= ~(1 << clockPin);
  _delay_ms(5);
};

void readInput(unsigned char *mkInput, unsigned char *brInput) {
  unsigned int matrixInput = 0;
  // lower Latch pin to load data in
  PORTB |= (1 << CLK_INH);
  PORTD &= ~(1 << LAT_IN);
  _delay_ms(5);
  PORTD |= (1 << LAT_IN);
  PORTB &= ~(1 << CLK_INH);
  _delay_ms(5);

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

void scanMatrix(void (*scanRow)(unsigned char t, unsigned char mk,
                                unsigned char br)) {
  // queue up high into the shift register
  PORTD |= (1 << SFT_OUT);

  for (int t = 0; t < 8; t++) {
    // pulse the clock to load the next queued up value into the shift
    // register
    pulseClock(CLK_OUT);

    unsigned char mkInput = 0;
    unsigned char brInput = 0;
    readInput(&mkInput, &brInput);
    (*scanRow)(t, mkInput, brInput);

    // queue up high into the shift register
    PORTD &= ~(1 << SFT_OUT);
  }
};