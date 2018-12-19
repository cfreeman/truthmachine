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
#include <Wire.h>

#define RRTHRESH 3

// TODO: Find sinusoidal function instead of state machine approach.

typedef struct Smoothed_struct {
  static const int maxValues = 10;
  int numValues = 0;
  int values[maxValues] = { 0 };
  int idx = 0;
  int sum = 0;

  int smoothed_value = 0;
} SmoothedValues;

void add_value(SmoothedValues *sv, int v) {
  sv->sum = sv->sum - sv->values[sv->idx] + v;

  sv->values[sv->idx] = v;
  sv->idx = (sv->idx + 1) % sv->maxValues;

  if (sv->numValues <= sv->maxValues) {
    sv->numValues++;
  }

  sv->smoothed_value = sv->sum / sv->numValues;
}

typedef struct RRStateStruct (*RRModeFn)(struct RRStateStruct current_state,
                                         int chest_pos,
                                         unsigned long current_time);

typedef struct RRStateStruct {
  int last_chest_pos;     // The last maxima or minima chest position.
  unsigned long last_t;   // The last time we reached a max chest position.
  SmoothedValues breaths; // Bucket of the last ten breaths.
  int bpm;                // Current respiratory rate in breaths per minute.

  RRModeFn updateRR;
} RRState;

RRState BreatheIn(RRState current_state,
                  int chest_pos,
                  unsigned long current_time);

RRState BreatheOut(RRState current_state,
                   int chest_pos,
                   unsigned long current_time);


RRState BreatheIn(RRState current_state,
                  int chest_pos,
                  unsigned long current_time) {
  int delta_cp = chest_pos - current_state.last_chest_pos;
  int new_cp = (chest_pos < current_state.last_chest_pos) ? chest_pos : current_state.last_chest_pos;

  if (abs(delta_cp) <= RRTHRESH || delta_cp < -RRTHRESH) {
    return {new_cp, current_state.last_t, current_state.breaths, current_state.bpm, &BreatheIn};
  }

  // have reached our chest peek, count the breath.
  Serial.print("Breath cp: ");
  Serial.print(chest_pos);
  Serial.print(" lcp:");
  Serial.print(current_state.last_chest_pos);
  Serial.print(" ncp:");
  Serial.print(new_cp);
  Serial.print(" dlcp:");
  Serial.println(delta_cp);

  Serial.print("BT: ");
  Serial.println(current_time - current_state.last_t);

  add_value(&current_state.breaths, (int)(current_time - current_state.last_t));
  current_state.bpm = 60000 / current_state.breaths.smoothed_value;

  Serial.print("****");
  Serial.print(current_state.bpm);
  Serial.println("BPM");

  return {new_cp, current_time, current_state.breaths, current_state.bpm, &BreatheOut};
}

RRState BreatheOut(RRState current_state,
                   int chest_pos,
                   unsigned long current_time) {
  int delta_cp = chest_pos - current_state.last_chest_pos;
  int new_cp = (chest_pos > current_state.last_chest_pos) ? chest_pos : current_state.last_chest_pos;

  if (abs(delta_cp) <= RRTHRESH || delta_cp > RRTHRESH) {
    return {new_cp, current_state.last_t, current_state.breaths, current_state.bpm, &BreatheOut};
  }

  // Serial.print("BreatheOut to In cp: ");
  // Serial.print(chest_pos);
  // Serial.print(" lcp:");
  // Serial.print(current_state.last_chest_pos);
  // Serial.print(" ncp:");
  // Serial.print(new_cp);
  // Serial.print(" dcp:");
  // Serial.println(delta_cp);

  return {new_cp, current_state.last_t, current_state.breaths, current_state.bpm, &BreatheIn};
}


RRState rr_state;
SmoothedValues rr_sensor;
SmoothedValues gsr_sensor;
SmoothedValues hr_sensor;

// setup configures the underlying hardware for use in the main loop.
void setup() {
  delay(5000);
  Serial.begin(9600);

  Serial.print("****");
  Serial.println(v);
  rr_state = {v, millis(), SmoothedValues{}, 0, &BreatheOut};

  Wire.begin();
}

// loop executes over and over on the microcontroller.
void loop() {
  // Get the latest data from the Respiration Rate and Galvanic Skin Response Sensors.
  add_value(&gsr_sensor, analogRead(A0));
  add_value(&rr_sensor, analogRead(A1));

  // Get the latest data from the Heart Rate Sensor.
  Wire.requestFrom(0xA0 >> 1, 1);
  while(Wire.available()) {
    unsigned char c = Wire.read();
    Serial.println(c, DEC);
  }

  delay(500);

  // rr_state = rr_state.updateRR(rr_state, rr_sensor.smoothed_value, millis());
  // delay(25);
}