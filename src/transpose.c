#include <avr/io.h>
#include <stdbool.h>
#include <stdlib.h>

#include "midi.h"
#include "millis.h"
#include "transpose.h"
#include "wiring.h"

#define OCTAVE 12
#define MIDI_RANGE 127
#define NUM_KEYS 37
#define DEBOUNCE_TIME 100

static unsigned int lastDebounceTimes[2] = {0};
static bool lastButtonState[2] = {0};
static bool buttonState[2] = {0};
static unsigned char offset = 48;

void initTranspose(void) {
  DDRB &= ~(1 << BUTTON_A) & ~(1 << BUTTON_B);

  // enable pullup
  PORTB |= (1 << BUTTON_A) | (1 << BUTTON_B);
};

void processButton(unsigned char index) {
  // inputs enable low
  bool currentState = !(PINB & (1 << index));

  if (currentState != lastButtonState[index]) {
    lastDebounceTimes[index] = millis();
  }

  if (millis() - lastDebounceTimes[index] > DEBOUNCE_TIME) {
    if (currentState != buttonState[index]) {
      if (index == BUTTON_A) {
        if (offset > OCTAVE) {
          offset -= OCTAVE;
        }
      } else {
        if (offset <= MIDI_RANGE - NUM_KEYS - OCTAVE) {
          offset += OCTAVE;
        }
      }
    }
    buttonState[index] = currentState;
  }

  lastButtonState[index] = currentState;
};

void processTranspose(void) {
  processButton(BUTTON_A);
  processButton(BUTTON_B);
};

unsigned char getTransposeOffset(void) { return offset; };