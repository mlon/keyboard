#include <avr/io.h>
#include <stdbool.h>
#include <stdlib.h>

#include "midi.h"
#include "millis.h"
#include "transpose.h"
#include "wiring.h"

static const unsigned int DELAY_FASTEST = 5;
static unsigned char offset[37] = {0};

void sendByte(unsigned char data) {
  while (!(UCSR0A & (1 << UDRE0))) {
  };
  UDR0 = data;
}

void sendChannelPressure(unsigned char channelPressure) {
  sendByte(0xD0);
  sendByte(channelPressure / 2);
};

void sendKey(unsigned char row, unsigned char col, bool on,
             unsigned int delay) {

  /*
    sendByte(0xF0);
    sendByte(0x42);
    sendByte(row);
    sendByte(col);
    sendByte(0xF7);
  */
  unsigned char key = col * 8 + row;
  offset[key] = on ? getTransposeOffset() : offset[key];

  sendByte(on ? 0x90 : 0x80);
  sendByte(offset[key] + key);
  unsigned char velocity = 127 * DELAY_FASTEST / delay;
  sendByte(velocity < 127 ? velocity : 127);
};

void initMidi(void) {
  DDRD |= (1 << TX);

  // set up UART
  UCSR0B |= (1 << TXEN0);
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

  unsigned int ubrr = (F_CPU / (16UL * 31250UL)) - 1;
  UBRR0H = (unsigned char)(ubrr >> 8);
  UBRR0L = (unsigned char)ubrr;
}