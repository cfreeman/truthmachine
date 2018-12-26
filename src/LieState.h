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
#ifndef _LIE_STATE_C_ACH_
#define _LIE_STATE_C_ACH_

#define MEASURE_DURATION 5000   // The duration of the lie detection process. 
#define LOG_LENGTH 10           // The number of datapoints to collect for calculating lie likelyhood

typedef struct LieStateStruct (*LieModeFn)(struct LieStateStruct current_state,
                                           bool interrogated,
                                           int respiratory_rate,
                                           int heart_rate,
                                           int galvanic_skin_response,
                                           unsigned long current_time);

typedef struct LieStateStruct {

  int rr_delta_t[LOG_LENGTH];
  int hr_delta_t[LOG_LENGTH];
  int gsr_delta_t[LOG_LENGTH];

  unsigned long stateStart;   // The time in milliseconds when the current updateRR mode started.

  LieModeFn updateRR;
} RRState;

LieStateStruct idle(LieStateStruct current_state,
                    bool interrogated,
                    int respiratory_rate,
                    int heart_rate,
                    int galvanic_skin_response,
                    unsigned long current_time);

LieStateStruct measure(LieStateStruct current_state,
                       bool interrogated,
                       int respiratory_rate,
                       int heart_rate,
                       int galvanic_skin_response,
                       unsigned long current_time);

LieStateStruct log(LieStateStruct current_state,
                   bool interrogated,
                   int respiratory_rate,
                   int heart_rate,
                   int galvanic_skin_response,
                   unsigned long current_time);

LieStateStruct report(LieStateStruct current_state,
                      bool interrogated,
                      int respiratory_rate,
                      int heart_rate,
                      int galvanic_skin_response,
                      unsigned long current_time);

#endif