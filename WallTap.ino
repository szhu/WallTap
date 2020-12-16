#include <stdarg.h>

#define DEBUG 0

/*
 * Circuits:
 * A0 -> 10kohm -> LED -> GND
 * A0 -> 10~30Mohm -> Hand
 * A0 -> 10~30Mohm -> A2 -> D transistor S -> GND
 * A1 -> G transistor
 */

struct Stats {
  float mean;
  float stdev;
};

/**
 * Keeps track of a running average and stdev.
 * https://stackoverflow.com/a/17637351/782045
 */
class RunningStats {
  float n = 0;
  float old_m = 0;
  float new_m = 0;
  float old_s = 0;
  float new_s = 0;

public:
  void push(float x) {
    n += 1;

    if (n == 1) {
      old_m = new_m = x;
      old_s = 0;
    } else {
      new_m = old_m + (x - old_m) / n;
      new_s = old_s + (x - old_m) * (x - new_m);

      old_m = new_m;
      old_s = new_s;
    }
  }

  float mean() { return new_m; }

  float variance() { return n > 1 ? new_s / (n - 1) : 0.0; }

  float stdev() { return sqrt(variance()); }

  struct Stats stats() {
    struct Stats stats {
      mean(), stdev()
    };
    return stats;
  }
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

#define PIN_INPUT A0
#define PIN_DRAIN A1
#define PIN_OUTPUT A2

struct Stats measure(int reps, int resetEvery) {
  RunningStats running = RunningStats();

  for (int i = 0; i < reps; i++) {
    if (i % resetEvery == 0) {
      analogWrite(PIN_DRAIN, 1023);
      delay(5);
      analogWrite(PIN_DRAIN, 0);
      delay(1);
    }

    int level = analogRead(PIN_OUTPUT);
    running.push((float)level);

#if DEBUG
    printMeter(level, 1023);
    Serial.println();
#endif
  }

  return running.stats();
}

int pressedInARow = 0;

#define MEASURE_REPS 50
#define MEASURE_RESET_EVERY 5

float TRIGGER_LEVEL;
float DEBUG_LEVEL;
struct Stats calibrated;

/**
 * Entry point functions
 */

void setup() {
  //
  analogWrite(PIN_INPUT, 1023);

  delay(2000);

  // Calibrate
  RunningStats running = RunningStats();
  for (int i = 0; i < 20; i++) {
    struct Stats singleRunStats = measure(MEASURE_REPS, MEASURE_RESET_EVERY);
    float level = singleRunStats.mean;
    p(R"({"type": "calibration", "level": %i})", (int)level);
    p("\n");

    running.push(level);

    delay(random(0, 300));
  }

  calibrated = running.stats();
  TRIGGER_LEVEL = (calibrated.mean + 1.9 * calibrated.stdev);
  DEBUG_LEVEL = (calibrated.mean + 1.3 * calibrated.stdev);

  p(R"({"type": "calibrated", "mean": %i, "stdev": %i, "trigger": %i, "debug": %i})",
    (int)calibrated.mean, (int)calibrated.stdev, (int)TRIGGER_LEVEL,
    (int)DEBUG_LEVEL);
  p("\n");
}

void loop() {
  struct Stats stats = measure(MEASURE_REPS, MEASURE_RESET_EVERY);

  int level = stats.mean;

#if DEBUG
  p("<- %i %i ", level > TRIGGER_LEVEL, level);
  printMeter(level, 1023);
  Serial.println();
#endif

  if (level > TRIGGER_LEVEL) {
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

  if (level >= DEBUG_LEVEL && level < TRIGGER_LEVEL) {
    float x = (stats.mean - calibrated.mean) / calibrated.stdev;
    p(R"({"type": "debug", "level": %i, "x": %i})", level, (int)(x * 10));
    p("\n");
  }

#if DEBUG
  Serial.println();
  delay(500);
#endif
}
