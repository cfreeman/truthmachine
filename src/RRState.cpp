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
                  float chest_pos,
                  unsigned long current_time) {

  // chest_pos decreases as you breathe in.
  float minima = min(chest_pos, current_state.last_chest_pos);

  if ((current_state.last_chest_pos - chest_pos) < -RRTHRESH) {
    float breath_length = ((current_time - current_state.last_t) / 1000.0);

    add_value(current_state.breaths, breath_length);
    current_state.bpm = (int) ceil((60.0 / current_state.breaths->smoothed_value));

    return {minima, current_time, current_state.breaths, current_state.bpm, &BreatheOut};
  }

  return {minima, current_state.last_t, current_state.breaths, current_state.bpm, &BreatheIn};
}

RRState BreatheOut(RRState current_state,
                   float chest_pos,
                   unsigned long current_time) {

  // chest_pos increases as you breathe out.
  float maxima = max(chest_pos, current_state.last_chest_pos);

  if ((current_state.last_chest_pos - chest_pos) > RRTHRESH) {
    return {maxima, current_state.last_t, current_state.breaths, current_state.bpm, &BreatheIn};
  }

  return {maxima, current_state.last_t, current_state.breaths, current_state.bpm, &BreatheOut};
}