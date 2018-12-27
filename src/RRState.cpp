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
#include "RRState.h"

RRState BreatheIn(RRState current_state,
                  int chest_pos,
                  unsigned long current_time) {

  int delta_cp = chest_pos - current_state.last_chest_pos;
  int new_cp = (chest_pos < current_state.last_chest_pos) ? chest_pos : current_state.last_chest_pos;

  if (abs(delta_cp) <= RRTHRESH || delta_cp < -RRTHRESH) {
    return {new_cp, current_state.last_t, current_state.breaths, current_state.bpm, &BreatheIn};
  }

  // have reached our chest peek, count the breath.
  // Serial.print("Breath cp: ");
  // Serial.print(chest_pos);
  // Serial.print(" lcp:");
  // Serial.print(current_state.last_chest_pos);
  // Serial.print(" ncp:");
  // Serial.print(new_cp);
  // Serial.print(" dlcp:");
  // Serial.println(delta_cp);

  // Serial.print("BT: ");
  // Serial.println(current_time - current_state.last_t);

  add_value(current_state.breaths, (int)(current_time - current_state.last_t));
  current_state.bpm = 60000 / current_state.breaths->smoothed_value;

//  Serial.print("****");
  // Serial.print(current_state.bpm);
  // Serial.println("BPM");

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