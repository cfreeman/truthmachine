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
#include <Wire.h>

#include <Bridge.h>
#include <BridgeServer.h>
#include <BridgeClient.h>

#include "SmoothedStruct.h"
#include "RRState.h"

/**

TODO:
 * Tidy up the RRState machine and stop counting false positives.
 * Add the ability to calibrate and set baseline responses.
 * Finish up mechanics for sending and recieving data.
 * Calculate lie likelyhood.
 * Implement other broadcast information.
*/ 



BridgeServer server;

RRState rr_state;
SmoothedValues *rr_sensor;
SmoothedValues *gsr_sensor;
int heartRate;
//SmoothedValues hr_sensor;

// setup configures the underlying hardware for use in the main loop.
void setup() {
  delay(1000);
  
  
  Serial.begin(9600);
  Serial.println("Booting...");

  Bridge.begin();
  server.listenOnLocalhost();
  server.begin();
  
  Wire.begin();
  

  rr_sensor = new_smoothed(10);   // Exists till hard recycle.
  gsr_sensor = new_smoothed(10);  // Exists till hard recycle.

  heartRate = 0;

  add_value(gsr_sensor, analogRead(A0));
  add_value(rr_sensor, analogRead(A1));

  rr_state = {gsr_sensor->smoothed_value, millis(), rr_sensor, 0, &BreatheOut};

  Serial.println("***Booted");
}



// Transmits:
// /lielikelyhood - A normalised value between 0.0 and 1.0 for how stressed / more likely a person is to be lying. (triggered by an /interrogate message).

// /hr - an integer value that is the current heart rate of the participant in beats per minute
// /rr - an integer value that is the current respiratory rate of the participant in breaths per minute.
// /gsr - a floating point value that is the current electrodermal activity in microsiemens. 

// /pulse - a message that is sent 'n' times a minute where n is the current heart rate in beats per minute. 
void transmit() {
  
  // Spin up a curl process on the atheros processor running linino.
  Process p;
  p.begin("curl");

  // Add the argument to the end of the URL.
  //dtostrf(current_dispenser_level, 5, 5, &url_buffer[29]);
  //Serial.println(url_buffer);
  //p.addParameter(url_buffer); // Add the URL parameter to "curl"
  p.run();
}

// Recieves:
// /interrogate - When the sensor receives this message it monitors the biometric sensors for ten seconds (configurable) and calculates the lie likely hood. When completed the sensor transmits a /lielikelyhood message back.
bool measure_interrogation(BridgeClient client) {
  // ADDRESSS: 192.168.0.6/arduino/drain/0.55

  String command = client.readStringUntil('/');
  command.trim();

  return (command == "interrogate");
}

// loop executes over and over on the microcontroller.
void loop() {
  // Get the latest data from the Respiration Rate and Galvanic Skin Response Sensors.
  add_value(gsr_sensor, analogRead(A0));
  add_value(rr_sensor, analogRead(A1));
  rr_state = rr_state.updateRR(rr_state, rr_sensor->smoothed_value, millis());                                       

  // Get the latest data from the Heart Rate Sensor.
  Wire.requestFrom(0xA0 >> 1, 1);
  while(Wire.available()) {
    heartRate = (int) Wire.read();
    Serial.println(heartRate);
  }

  BridgeClient client = server.accept();
  if (client) {
    // Trigger lie measurement.
    measure_interrogation(client);
    client.stop();
  }

  delay(500);
}