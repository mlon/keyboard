#include <avr/io.h>
#include <stdbool.h>

#include "after_touch.h"
#include "midi.h"
#include "wiring.h"

unsigned int previousAfterTouch = 0;

void initAfterTouch(void) {
  DDRC |= (1 << AFTER_TOUCH);
  DIDR0 |= (1 << AFTER_TOUCH);

  // set up ADC, set prescalar to 128,11.0598MHz/128 = 85KHz
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);
  ADMUX |= (1 << REFS0);
};

void processAfterTouch(void) {
  ADMUX |= AFTER_TOUCH;
  ADCSRA |= (1 << ADSC);
  while (!(ADCSRA & (1 << ADSC))) {
  };

  unsigned int afterTouch = ADC;
  // TODO process afterTouch

  if (afterTouch != previousAfterTouch) {
    previousAfterTouch = afterTouch;
    sendChannelPressure(previousAfterTouch);
  }
};