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
#include <stdlib.h>
#include "SmoothedStruct.h"

SmoothedValues* new_smoothed(int size) {
	SmoothedValues* result = (SmoothedValues*) calloc(1, sizeof(SmoothedValues));

	result->maxValues = size;
	result->numValues = 0;
	result->values = (int*) calloc(size, sizeof(int));
	result->idx = 0;
	result->sum = 0;
	result->smoothed_value = 0;

	return result;
}

void free_smoothed(SmoothedValues *sv) {
	free(sv->values);
	free(sv);
}

void add_value(SmoothedValues *sv, int v) {
  sv->sum = sv->sum - sv->values[sv->idx] + v;

  sv->values[sv->idx] = v;
  sv->idx = (sv->idx + 1) % sv->maxValues;

  if (sv->numValues <= sv->maxValues) {
    sv->numValues++;
  }

  sv->smoothed_value = sv->sum / sv->numValues;
}