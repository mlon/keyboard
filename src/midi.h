#pragma once

#include <stdbool.h>
#include <stdlib.h>

static const unsigned int DELAY_FASTEST = 5;

void initMidi(void);

void sendChannelPressure(unsigned int channelPressure);
void sendKey(unsigned char row, unsigned char col, bool on, unsigned int delay);