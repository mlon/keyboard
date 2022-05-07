#include <avr/io.h>
#include <util/delay.h>

#include "voices.h"

#define BLINK_DELAY_MS 1000

uint64_t counter = 0;

int turn_on(uint64_t counter) { return counter % 3; }

int main(void) {
  DDRB |= _BV(DDB5);

  while (1) {
    counter++;

    if (turn_on(counter) && !turn_on(counter - 1)) {
      PORTB |= _BV(PORTB5);
    }

    if (!turn_on(counter) && turn_on(counter - 1)) {
      PORTB &= ~_BV(PORTB5);
    }
  }
}