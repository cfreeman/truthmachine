/*
 * Copyright (c) Clinton Freeman 2018
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <Arduino.h>

typedef struct Smoothed_struct {
  static const int numValues = 10;
  int values[numValues] = { 0 };
  int Idx = 0;
  int sum = 0;

  int smoothed_value = 0;
} smoothed_values;

void add_value(smoothed_values *sv, int v) {
  sv->sum = sv->sum - sv->values[sv->Idx] + v;

  sv->values[sv->Idx] = v;
  sv->Idx = (sv->Idx + 1) % sv->numValues;

  sv->smoothed_value = sv->sum / sv->numValues;
}



smoothed_values rr_sensor;

// setup configures the underlying hardware for use in the main loop.
void setup() {
  Serial.begin(9600);
}

// loop executes over and over on the microcontroller.
void loop() {
  add_value(&rr_sensor, analogRead(4));

  Serial.println(rr_sensor.smoothed_value);

  delay(100);
}
