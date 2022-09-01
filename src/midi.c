#include <avr/io.h>
#include <stdbool.h>
#include <stdlib.h>

#include "midi.h"

#define TX 1

void sendByte(char data) {
  while (!(UCSR0A & (1 << UDRE0))) {
  };
  UDR0 = data;
}

void sendChannelPressure(unsigned int channelPressure) {
  sendByte(0xD0);
  sendByte(channelPressure / 2);
};

void sendKey(unsigned char row, unsigned char col, bool on,
             unsigned int delay) {

  sendByte(on ? 0x90 : 0x80);
  sendByte(col * 5 + row);
  unsigned char velocity = 127 * DELAY_FASTEST / delay;
  sendByte(velocity < 127 ? velocity : 127);
};

void initMidi(void) {
  DDRD |= (1 << TX);

  // set up UART
  UCSR0B |= (1 << TXEN0);
  unsigned int ubrr = (F_CPU / (16 * 31250UL)) - 1;
  UBRR0L = (unsigned char)(ubrr >> 8);
  UBRR0H = (unsigned char)ubrr;
}