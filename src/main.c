#include <stdbool.h>
#include <stdlib.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

#include "millis.h"

// PortB 5
// 0-4 Break Input

// PortC
// 0-4 Make Input
#define AFTER_TOUCH 5

// PortD  2, 3, 4
#define RX 0
#define TX 1
#define CLOCK 5
#define SHIFT 6
#define CLEAR 7

#define setBit(p, m) ((p) |= _BV((m)))
#define clearBit(p, m) ((p) &= ~_BV((m)))
#define getBit(p, m) ((p)&_BV(m))

unsigned char mkStates[8] = {0};
unsigned char brStates[8] = {0};
unsigned int times[8][5] = {0};

void sendByte(char data) {
  while (!(UCSR0A & (1 << UDRE0))) {
  };
  UDR0 = data;
}

void sendChannelPressure(unsigned int channelPressure){};
void sendKey(char row, char col, bool on, unsigned int delay){};

int main(void) {
  init_millis();

  // set up IO
  DDRD |= (1 << TX) | (1 << CLOCK) | (1 << CLEAR) | (1 << SHIFT);
  DDRC |= (1 << AFTER_TOUCH);

  // set up UART
  UCSR0B |= (1 << TXEN0);
  unsigned int ubrr = (F_CPU / (16 * 31250UL)) - 1;
  UBRR0L = (unsigned char)(ubrr >> 8);
  UBRR0H = (unsigned char)ubrr;

  // set up ADC, set prescalar to 128,11.0598MHz/128 = 85KHz
  unsigned int afterTouch = 0;
  ADMUX |= AFTER_TOUCH;
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);

  // reset T shift register
  PORTD &= ~(1 << CLEAR);
  PORTD |= (1 << CLOCK);
  PORTD &= ~(1 << CLOCK);
  PORTD |= (1 << CLEAR);

  while (true) {
    // queue up high into the shift register
    PORTD |= (1 << SHIFT);
    for (char i = 0; i < 8; i++) {
      // pulse the clock to load the next queued up value into the shift
      // register
      PORTD |= (1 << CLOCK);
      PORTD &= ~(1 << CLOCK);

      for (char j = 0; j < 5; j++) {
        if (getBit(PINB, j)) {
          // break

          if (!getBit(brStates[i], j)) {
            setBit(brStates[i], j);
            times[i][j] = millis();
          }

          if (getBit(PINC, j)) {
            // make
            if (!getBit(mkStates[i], j)) {
              sendKey(i, j, true, millis() - times[i][j]);
              setBit(mkStates[i], j);
            }
          } else {
            if (getBit(mkStates[i], j)) {
              clearBit(mkStates[i], j);
              times[i][j] = millis();
            }
          }
        } else {
          if (getBit(brStates[i], j)) {
            sendKey(i, j, false, millis() - times[i][j]);
            clearBit(brStates[i], j);
          }
        }
      }

      // queue up low into the shift register
      PORTD &= ~(1 << SHIFT);
    }

    ADCSRA |= (1 << ADSC);
    while (!(ADCSRA & (1 << ADSC))) {
    };
    if (afterTouch != ADC) {
      afterTouch = ADC;
      sendChannelPressure(afterTouch);
    }
  }
}