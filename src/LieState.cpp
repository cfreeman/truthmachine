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
#include "LieState.h"
#include "Transmit.h"

LieState copyLieState(const LieState *current_state,
                      bool includeInterrogation,
                      LieModeFn updateFN) {
  LieState newLieState = {{0}, {0}, {0}, {0}, {0}, {0},
                          current_state->calibrationPoints,
                          current_state->stateStart,
                          updateFN};

  for (uint8_t i = 0; i < LOG_LENGTH; i++) {
    if (includeInterrogation) {
      newLieState.rr_delta_t[i] = current_state->rr_delta_t[i];
      newLieState.hr_delta_t[i] = current_state->hr_delta_t[i];
      newLieState.gs_delta_t[i] = current_state->gs_delta_t[i];
    }

    newLieState.rr_delta_t_calibration[i] = current_state->rr_delta_t_calibration[i];
    newLieState.hr_delta_t_calibration[i] = current_state->hr_delta_t_calibration[i];
    newLieState.gs_delta_t_calibration[i] = current_state->gs_delta_t_calibration[i];
  }

  return newLieState;
}

LieState Idle(LieState current_state,
              char command,
              int respiratory_rate,
              int heart_rate,
              int galvanic_skin_response,
              unsigned long current_time) {

  if (command == 'i') {
    LieState newLieState = copyLieState(&current_state, false, &Measure);
    newLieState.stateStart = current_time;
    return newLieState;

  } else if (command == 'c') {
    LieState newLieState = copyLieState(&current_state, false, &Calibrate);
    newLieState.stateStart = current_time;
    return newLieState;

  } else if (command == 'r') {
    LieState newLieState = copyLieState(&current_state, false, &Reset);
    newLieState.stateStart = current_time;
    return newLieState;

  }

  return current_state;
}

LieState Measure(LieState current_state,
                 char command,
                 int respiratory_rate,
                 int heart_rate,
                 int galvanic_skin_response,
                 unsigned long current_time) {

  if (command == 'r') {
    LieState newLieState = copyLieState(&current_state, false, &Reset);
    newLieState.stateStart = current_time;
    return newLieState;
  }

  if (current_time > (current_state.stateStart + (MEASURE_DURATION / LOG_LENGTH))) {
    LieState newLieState = copyLieState(&current_state, true, &Log);

    return newLieState;
  }

  return current_state;
}

LieState Log(LieState current_state,
             char command,
             int respiratory_rate,
             int heart_rate,
             int galvanic_skin_response,
             unsigned long current_time) {

  if (command == 'r') {
    LieState newLieState = copyLieState(&current_state, false, &Reset);
    newLieState.stateStart = current_time;
    return newLieState;
  }

  LieState newLieState = copyLieState(&current_state, true, &Measure);

  uint8_t i = LOG_LENGTH * (uint8_t)((current_time - current_state.stateStart) / (float) MEASURE_DURATION);
  i = min(LOG_LENGTH, i);

  newLieState.rr_delta_t[i] = respiratory_rate;
  newLieState.hr_delta_t[i] = heart_rate;
  newLieState.gs_delta_t[i] = galvanic_skin_response;

  // If we have filled our delta_t logs, switch to report mode.
  if (current_time >= (current_state.stateStart + MEASURE_DURATION)) {
    newLieState.stateStart = current_time;
    newLieState.updateLS = &Report;
  }

  return newLieState;
}

float gradient(int dataSet[LOG_LENGTH]) {
  int sumX = 0;
  int sumY = 0;
  int sumXY = 0;
  int sumXX = 0;
  int sumYY = 0;

  for (int i = 0; i < LOG_LENGTH; i++) {
    sumX = sumX + i;
    sumY = sumY + dataSet[i];
    sumXY = sumXY + (i * dataSet[i]);
    sumXX = sumXX + (i * i);
    sumYY = sumYY + (dataSet[i] * dataSet[i]);
  }

  return ((LOG_LENGTH * sumXY) - (sumX * sumY)) / (float)((LOG_LENGTH * sumXX) - (sumXX * sumXX));
}

LieState Report(LieState current_state,
                char command,
                int respiratory_rate,
                int heart_rate,
                int galvanic_skin_response,
                unsigned long current_time) {

  if (command == 'r') {
    LieState newLieState = copyLieState(&current_state, false, &Reset);
    newLieState.stateStart = current_time;
    return newLieState;
  }

  // Calculate calibration gradients.
  int caliR[LOG_LENGTH];
  int caliH[LOG_LENGTH];
  int caliG[LOG_LENGTH];
  for (unsigned int i = 0; i < LOG_LENGTH; i++) {
    caliR[i] = current_state.rr_delta_t_calibration[i] / current_state.calibrationPoints;
    caliH[i] = current_state.hr_delta_t_calibration[i] / current_state.calibrationPoints;
    caliG[i] = current_state.gs_delta_t_calibration[i] / current_state.calibrationPoints;
  }

  float trendCR = gradient(caliR);
  float trendCH = gradient(caliH);
  float trendCG = gradient(caliG);

  float calibrate_trend = (trendCR + trendCH + (2 * trendCG)) / 4.0;

  // Calculate lie likelyhood
  float trendR = gradient(current_state.rr_delta_t);
  float trendH = gradient(current_state.hr_delta_t);
  float trendG = gradient(current_state.gs_delta_t);

  float lie_likely_hood = ((trendR + trendH + (2 * trendG)) / 4.0) - calibrate_trend;

  transmit('h', heart_rate);
  transmit('r', respiratory_rate);
  transmit('g', galvanic_skin_response);
  transmit('l', lie_likely_hood);

  LieState newLieState = copyLieState(&current_state, false, &Idle);

  return newLieState;
}

LieState Calibrate(LieState current_state,
                   char command,
                   int respiratory_rate,
                   int heart_rate,
                   int galvanic_skin_response,
                   unsigned long current_time) {

  if (command == 'r') {
    LieState newLieState = copyLieState(&current_state, false, &Reset);
    newLieState.stateStart = current_time;
    return newLieState;
  }

  if (current_time > (current_state.stateStart + (MEASURE_DURATION / LOG_LENGTH))) {
    LieState newLieState = copyLieState(&current_state, false, &LogCalibration);

    return newLieState;
  }

  return current_state;
}

LieState LogCalibration(LieState current_state,
                        char command,
                        int respiratory_rate,
                        int heart_rate,
                        int galvanic_skin_response,
                        unsigned long current_time) {

  if (command == 'r') {
    LieState newLieState = copyLieState(&current_state, false, &Reset);
    newLieState.stateStart = current_time;
    return newLieState;
  }

  LieState newLieState = copyLieState(&current_state, false, &Calibrate);

  uint8_t i = LOG_LENGTH * (uint8_t)((current_time - current_state.stateStart) / (float) MEASURE_DURATION);
  i = min(LOG_LENGTH, i);

  newLieState.rr_delta_t_calibration[i] += respiratory_rate;
  newLieState.hr_delta_t_calibration[i] += heart_rate;
  newLieState.gs_delta_t_calibration[i] += galvanic_skin_response;

  // If we have filled our delta_t logs, switch to report mode.
  if (current_time >= (current_state.stateStart + MEASURE_DURATION)) {
    newLieState.stateStart = current_time;
    newLieState.calibrationPoints = newLieState.calibrationPoints + 1;

    newLieState.updateLS = &Idle;

    transmit('h', heart_rate);
    transmit('r', respiratory_rate);
    transmit('g', galvanic_skin_response);

    return newLieState;
  }

  return current_state;
}

LieState Reset(LieState current_state,
               char command,
               int respiratory_rate,
               int heart_rate,
               int galvanic_skin_response,
               unsigned long current_time) {

  return LieState {{0}, {0}, {0}, {0}, {0}, {0}, 0, current_time, &Idle};
}
