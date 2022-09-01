#include <stdbool.h>
#include <stdlib.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <math.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "matrix.h"
#include "midi.h"
#include "millis.h"

// PortB
#define CLK_INH 0

// PortC
#define AFTER_TOUCH 5

// PortD
#define CLK_IN 2
#define LAT_IN 3
#define OUT_IN 4
#define CLK_OUT 5
#define SFT_OUT 6
#define CLR_OUT 7

#define setBit(p, m) ((p) |= _BV((m)))
#define clearBit(p, m) ((p) &= ~_BV((m)))
#define getBit(p, m) ((p)&_BV(m))

unsigned char mkStates[8] = {0};
unsigned char brStates[8] = {0};
unsigned int times[8][5] = {0};

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

int main(void) {
  // Blink to debug
  DDRB |= (1 << 5);

  init_millis();

  // set up IO
  DDRC |= (1 << AFTER_TOUCH);

  // set up ADC, set prescalar to 128,11.0598MHz/128 = 85KHz
  unsigned int afterTouch = 0;
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);

  // set up matrix
  initMatrix();

  while (true) {
    scanMatrix(*scanRow);

    ADMUX |= AFTER_TOUCH;
    ADCSRA |= (1 << ADSC);
    while (!(ADCSRA & (1 << ADSC))) {
    };
    if (afterTouch != ADC) {
      afterTouch = ADC;
      sendChannelPressure(afterTouch);
    }
  }
}