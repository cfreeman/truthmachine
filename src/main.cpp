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
#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>
#include <Wire.h>

#include "LieState.h"
#include "RRState.h"
#include "SmoothedStruct.h"
#include "Transmit.h"

#define UPDATE_INTERVAL 3000

BridgeServer server;

RRState rr_state;
SmoothedValues *rr_sensor;
SmoothedValues *delta_t_breaths;

LieState lie_state;

unsigned long lastUpdate;

// setup configures the underlying hardware for use in the main loop.
void setup() {
  Bridge.begin();

  server.listenOnLocalhost();
  server.begin();

  Wire.begin();

  rr_sensor = new_smoothed(10);       // Exists till hard recycle.
  delta_t_breaths = new_smoothed(10); // Exists till hard recycle.

  add_value(rr_sensor, analogRead(A1));

  rr_state = RRState{rr_sensor->smoothed_value, millis(), delta_t_breaths, 0, &Initial};
  lie_state = LieState{{0}, {0}, {0}, {0}, {0}, {0}, 0, millis(), &Idle};

  lastUpdate = millis();
}

// Recieves:
// /interrogate - When the sensor receives this message it monitors the biometric sensors for ten seconds (configurable) and calculates the lie likely hood. When completed the sensor transmits a /lielikelyhood message back.
char get_command() {
  // ADDRESSES: http://10.0.1.3/arduino/interrogate
  //            http://10.0.1.3/arduino/calibrate

  BridgeClient client = server.accept();

  if (client && client.available()) {
    char c = (char) client.read();
    client.stop();
    return c;
  }

  return '.';
}

// loop executes over and over on the microcontroller.
void loop() {
  unsigned long t = millis();

  // Get the latest data from the Respiration Rate and Galvanic Skin Response Sensors.
  add_value(rr_sensor, analogRead(A1));
  rr_state = rr_state.updateRR(rr_state, rr_sensor->smoothed_value, t);

  // Get the latest data from the Heart Rate Sensor and broadcast it to the server.
  int heartRate = 60;  // Default the heart rate to a fairly average resting rate.
  Wire.requestFrom(0xA0 >> 1, 1);
  while(Wire.available()) {
    heartRate = (int) Wire.read();
  }

  if ((t - lastUpdate) > UPDATE_INTERVAL) {
    transmit('h', heartRate);
    transmit('g', analogRead(A0));
    transmit('r', rr_state.bpm);

    lastUpdate = t;
  }

  // Update the state of the theatrical polygraph.
  char c = get_command();
  lie_state = lie_state.updateLS(lie_state, c, rr_state.bpm, heartRate, analogRead(A0), t);

  delay(50);  // Give our microcontroller a little chill.
}