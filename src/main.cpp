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
  const int numValues = 10;
  int values[numValues];
  int valueIdx;
  int valueSum;

  int smoothed_value;
} smoothed_values;

smoothed_values init(int size) {
 smoothed_values result;

 result.values = { 0 };

 return result;
}

/*
int add_value(smoothed_values *sv, int v) {
}
*/

const int numReadings = 10;
int readings[numReadings];
int readIndex = 0;
//int total = 0;

// setup configures the underlying hardware for use in the main loop.
void setup() {
  Serial.begin(9600);
}

// loop executes over and over on the microcontroller.
void loop() {
  readings[readIndex] = analogRead(4);
  readIndex = (readIndex + 1) % numReadings;

  int sum = 0;
  for (int i = 0; i < numReadings; i++) {
    sum += readings[i];
  }

  Serial.println(sum/numReadings);

  delay(50);
}
