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

typedef struct LieStateStruct (*LieModeFn)(struct LieStateStruct current_state,
                                    	   int chest_pos,
                                           unsigned long current_time);

typedef struct LieStateStruct {

  // Array of heart rate results logged at half second intervals.
  // Array of GSR results logged at half second intervals.
  // Array of RR results logged at half second intervals.

  float lieLikelyHood;
  unsigned long stateStart;

  LieModeFn updateRR;
} RRState;

#endif