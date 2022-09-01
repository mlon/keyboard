#pragma once

void initMatrix(void);
void scanMatrix(void (*scanRow)(unsigned char t, unsigned char mk,
                                unsigned char br));