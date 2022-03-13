#include <Arduino.h>

static const double DELAY_FASTEST = 5;

static const u8 ROWS = 10;
static const u8 COLS = 8;

static const char *NOTES[12] = {"C",  "C#", "D",  "D#", "E",  "F",
                                "F#", "G",  "G#", "A",  "A#", "B"};

// make: evens
// break: odds
u8 rowPins[ROWS] = {40, 38, 36, 34, 32, 30, 28, 26, 24, 22};
bool activeRows[ROWS] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
u8 colPins[COLS] = {41, 39, 37, 35, 33, 31, 29, 27};

u8 rowsStates[ROWS] = {0};

u32 activatedTimestamp[ROWS][COLS] = {0};

void printNote(u8 row, u8 col) {
  char buffer[4];
  u16 index = (row / 2) * COLS + col;
  sprintf(buffer, "%s%d", NOTES[index % 12], index / 12);
  Serial.print(buffer);
}

void keyOn(u8 row, u8 col, u32 delay) {
  double velocity = DELAY_FASTEST / delay;
  if (velocity > 1) {
    velocity = 1;
  }
  printNote(row, col);
  Serial.print(" on (");
  Serial.print(velocity);
  Serial.println(")");
}

void keyOff(u8 row, u8 col) {
  printNote(row, col);
  Serial.println(" off");
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting");

  for (uint8_t row = 0; row < ROWS; row++) {
    pinMode(rowPins[row], OUTPUT);
    digitalWrite(rowPins[row], HIGH);
  }
  for (uint8_t col = 0; col < COLS; col++) {
    pinMode(colPins[col], INPUT_PULLUP);
  }
}

void scan() {
  u8 rowRead = 0;
  u8 rowState = 0;
  bool isMake;
  u8 rowChanged = 0;

  bool hasChanged;
  bool isPressed;
  u32 delay;

  for (uint8_t row = 0; row < ROWS; row++) {
    // skip make scanning if there is no active break on that row
    if (!activeRows[row]) {
      continue;
    }

    isMake = (row % 2 == 0);
    rowRead = 0;
    rowState = rowsStates[row];
    rowChanged = 0;

    digitalWrite(rowPins[row], LOW);

    for (uint8_t col = 0; col < COLS; col++) {
      if (digitalRead(colPins[col]) == LOW) {
        rowRead |= 1 << col;
      }
    }

    rowsStates[row] = rowRead;
    rowChanged = rowState ^ rowRead;

    if (rowChanged) {
      for (uint8_t col = 0, mask = 0x01; col < COLS; col++, mask <<= 1) {
        hasChanged = ((rowChanged & mask) != 0);
        isPressed = ((rowRead & mask) != 0);

        if (hasChanged) {
          if (isPressed) {
            activatedTimestamp[row][col] = millis();
            if (isMake) {
              delay = activatedTimestamp[row][col] -
                      activatedTimestamp[row + 1][col];
              keyOn(row, col, delay);
            } else {
              activeRows[row - 1] = true;
            }
          } else {
            if (isMake) {
              keyOff(row, col);
            } else {
              activeRows[row - 1] = false;
            }
          }
        }
      }
    }

    digitalWrite(rowPins[row], HIGH);
  }
}

void loop() { scan(); }