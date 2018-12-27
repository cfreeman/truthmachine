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
#include "Transmit.h"

#include <Arduino.h>
#include <BridgeClient.h>


#define BUFFER_LEN 64
#define COM_POS 27
#define ARG_POS 31

// Transmits:
// l - Is the lielikelyhood, a normalised value between 0.0 and 1.0 for how stressed / more likely
//     a person is to be lying. (triggered by an /interrogate message).
// h - The current heart rate of teh participant in beats per minute.
// r - The current respiratory rate of the participant in breaths per minute.
// g - The electrodermal activity of the participant in microsiemens.
// p - The pulse of the participant and is transmitted 'n' times a minute where n is the current
//     heart rate in beats per minute.
void transmit(char command, float argument) {

  char buffer[BUFFER_LEN] = "http://192.168.86.103:8080/*?v=";
  buffer[COM_POS] = command;
  // Add the argument to the end of the URL.
  dtostrf(argument, 5, 5, &buffer[ARG_POS]);
  Serial.println(buffer);

  // Spin up a curl process on the atheros processor running linino.
  Process p;
  p.begin("curl");
  p.addParameter(buffer); // Add the URL parameter to "curl"
  p.run();

}