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
#ifndef _RR_STATE_C_ACH_
#define _RR_STATE_C_ACH_

#define RRTHRESH 3

#include "SmoothedStruct.h"

typedef struct RRStateStruct (*RRModeFn)(struct RRStateStruct current_state,
                                         int chest_pos,
                                         unsigned long current_time);

typedef struct RRStateStruct {
  int last_chest_pos;		// The last maxima or minima chest position.
  unsigned long last_t; 	// The last time we reached a max chest position.
  SmoothedValues *breaths;	// Bucket of the last ten breaths.
  int bpm;                	// Current respiratory rate in breaths per minute.

  RRModeFn updateRR;
} RRState;

RRState BreatheIn(RRState current_state,
                  int chest_pos,
                  unsigned long current_time);

RRState BreatheOut(RRState current_state,
                   int chest_pos,
                   unsigned long current_time);

#endif