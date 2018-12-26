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
#include "LieState.h"
#include <Arduino.h>

LieState idle(LieState current_state,
              bool interrogated,
              int respiratory_rate,
              int heart_rate,
              int galvanic_skin_response,
              unsigned long current_time) {
  if (interrogated) {
    return LieState{{0}, {0}, {0}, current_time, &measure};
  }

  return current_state;
}

LieState measure(LieState current_state,
                 bool interrogated,
                 int respiratory_rate,
                 int heart_rate,
                 int galvanic_skin_response,
                 unsigned long current_time) {

  if (current_time > (current_state.stateStart + (MEASURE_DURATION / LOG_LENGTH))) {
    LieState newLieState = {{0}, {0}, {0}, current_time, &log};
    for (uint8_t i = 0; i < LOG_LENGTH; i++) {
      newLieState.rr_delta_t[i] = current_state.rr_delta_t[i];
      newLieState.hr_delta_t[i] = current_state.hr_delta_t[i];
      newLieState.gs_delta_t[i] = current_state.gs_delta_t[i];
    }

    return newLieState;
  }

  return current_state;
}

LieState log(LieState current_state,
             bool interrogated,
             int respiratory_rate,
             int heart_rate,
             int galvanic_skin_response,
             unsigned long current_time) {

  LieState newLieState = {{0}, {0}, {0}, current_time, &measure};
  for (uint8_t i = 0; i < LOG_LENGTH; i++) {
      newLieState.rr_delta_t[i] = current_state.rr_delta_t[i];
      newLieState.hr_delta_t[i] = current_state.hr_delta_t[i];
      newLieState.gs_delta_t[i] = current_state.gs_delta_t[i];
    }

  uint8_t i = LOG_LENGTH * (uint8_t)((current_time - current_state.stateStart) / (float) MEASURE_DURATION);
  // TODO: Need to make sure we are not longer then logLENGTH. (min)
  newLieState.rr_delta_t[i] = respiratory_rate;
  newLieState.hr_delta_t[i] = heart_rate;
  newLieState.gs_delta_t[i] = galvanic_skin_response;
  newLieState.updateRR = &measure;

  // If we have filled our delta_t logs, switch to report mode.
  if (current_time >= (current_state.stateStart + MEASURE_DURATION)) {
    newLieState.updateRR = &report;
  }

  return newLieState;
}

LieState report(LieState current_state,
                bool interrogated,
                int respiratory_rate,
                int heart_rate,
                int galvanic_skin_response,
                unsigned long current_time) {

  // Calculate lie likelyhood
  // Push data to Audio visual machine.

  // drop back into the idle state.

  return current_state;
}
