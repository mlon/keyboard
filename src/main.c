#include <stdbool.h>
#include <stdlib.h>

#include <avr/interrupt.h>
#include <avr/io.h>
#include <math.h>
#include <util/atomic.h>
#include <util/delay.h>

#include "after_touch.h"
#include "matrix.h"
#include "midi.h"
#include "millis.h"

int main(void) {
  initMillis();
  initMidi();
  initMatrix();
  initAfterTouch();

  sei();

  while (true) {
    scanMatrix();
    processAfterTouch();
  }
}