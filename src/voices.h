#pragma once

#include <stdint.h>

#define numVoices 8

unsigned short getVoice(unsigned int key);

unsigned short isEnabled(unsigned short voice);
void enable(unsigned short voice);
void disable(unsigned short voice);