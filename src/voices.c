#include "voices.h"

unsigned short enabledVoices[numVoices] = {0};
unsigned short activeVoices[numVoices] = {0};
unsigned long long lastPlayed[numVoices] = {0};
unsigned int voiceAssignments[numVoices] = {0};

unsigned short getVoice(unsigned int key) { return 1; }

unsigned short isEnabled(unsigned short voice) {
  if (voice >= numVoices) {
    return 0;
  }

  return enabledVoices[voice];
};

void enable(unsigned short voice) {
  if (voice >= numVoices) {
    return;
  }
  enabledVoices[voice] = 1;
};

void disable(unsigned short voice) {
  if (voice >= numVoices) {
    return;
  }
  enabledVoices[voice] = 0;
};