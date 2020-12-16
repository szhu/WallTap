#include <stdarg.h>

#define DEBUG 0

/*
 * Circuits:
 * A0 -> 10kohm -> LED -> GND
 * A0 -> 10Mohm -> Hand
 * A0 -> 10Mohm -> A2 -> D transistor S -> GND
 * A1 -> G transistor
 */

/**
 * Keeps track of a running average.
 */
class RunningAvg {
public:
  int total = 0;
  int n = 0;

  void add(int value) {
    total += value;
    n++;
  }

  int get() { return total / n; };
};

/**
 * Prints a horizontal bar illustrating the input value.
 * It is scaled such that a value of max shows as a full bar.
 */
void printMeter(int value, int max) {
  value = map(value, 0, max, 0, 50);
  char out[] = "    1    2    3    4    5    6    7    8    9    X";
  for (int i = 0; i < 50; i++) {
    if (value >= i)
      out[i] = '#';
  }
  Serial.print("| ");
  Serial.print(out);
  Serial.print(" |");
}

/**
 * A printf implementation.
 * https://playground.arduino.cc/Main/Printf/
 */
#define PRINTF_BUF_SIZE 128
void p(char *fmt, ...) {
  char buf[PRINTF_BUF_SIZE];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, PRINTF_BUF_SIZE, fmt, args);
  va_end(args);
  Serial.print(buf);
}

/* Main code below */

int measure(int pin, int reps) {
  RunningAvg avg = RunningAvg();
  for (int i = 0; i < reps; i++) {
    int level = analogRead(pin);
    avg.add(level);

#if DEBUG
    printMeter(level, 1023);
    Serial.println();
#endif
  }

  return avg.get();
}

#define PIN_INPUT A0
#define PIN_DRAIN A1
#define PIN_OUTPUT A2

#define TRIGGER_LEVEL 700
#define DEBUG_LEVEL 720

int pressedInARow = 0;

void setup() {
  //
  analogWrite(PIN_INPUT, 1023);
}

void loop() {
  analogWrite(PIN_DRAIN, 1023);
  // analogWrite(PIN_INPUT, 0);

  delay(1);

  analogWrite(PIN_DRAIN, 0);
  // analogWrite(PIN_INPUT, 1023);

  int level = measure(PIN_OUTPUT, 100);

#if DEBUG
  p("<- %i %i ", level < TRIGGER_LEVEL, level);
  printMeter(level, 1023);
  Serial.println();
#endif

  if (level < TRIGGER_LEVEL) {
    pressedInARow++;
    p(R"({"type": "pressed", "times": %i, "level": %i})", pressedInARow, level);
    p("\n");
  } else {
    if (pressedInARow > 0) {
      p(R"({"type": "unpressed", "times": %i, "level": %i})", pressedInARow,
        level);
      p("\n");
      pressedInARow = 0;
    }
  }

  if (level <= DEBUG_LEVEL && level > TRIGGER_LEVEL) {
    p(R"({"type": "debug", "level": %i})", level);
    p("\n");
  }

#if DEBUG
  Serial.println();
  delay(500);
#endif
}
