#pragma once

#include <stdbool.h>
#include <stdlib.h>

void initMidi(void);

void sendChannelPressure(unsigned char channelPressure);
void sendKey(unsigned char row, unsigned char col, bool on, unsigned int delay);