#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/atomic.h>

#include "millis.h"

static volatile unsigned int milliseconds;
ISR(TIMER2_COMPA_vect) { ++milliseconds; }

void initMillis(void) {
  TCCR2A = (1 << WGM21);
  TCCR2B = (1 << CS22);
  TIMSK2 = (1 << OCIE2A);
  OCR2A = ((F_CPU / 64) / 1000);
  sei();
}

unsigned int millis(void) {
  unsigned int ms;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { ms = milliseconds; }
  return ms;
}