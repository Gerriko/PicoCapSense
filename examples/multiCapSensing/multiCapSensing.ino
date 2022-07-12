/*
 Copyright (c) 2022 C Gerrish (https://github.com/Gerriko)
 This example takes a rolling average to smooth out capacitance sampled values.
 * This example demonstrates the use of 3 Capacitive Sensors

 It uses a capacitive Sensing library for RP2040 based boards (using the PIO processor).
 Capacitive Sensing is used to detect touch based on the electrical
 capacitance of the human body.

 This library is based on the measuring methods used in the
 Capacitive Sensing Library for 'duino / Wiring (sampling method differs)
 https://github.com/PaulStoffregen/CapacitiveSensor
 Copyright (c) 2009 Paul Bagder
 Updates for other hardare by Paul Stoffregen, 2010-2016

 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal in the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS IN THE SOFTWARE.
 */

 #include "PicoCapSensing.h"

// define the pin numbers:
static const int TRIGPINS[3] = {13, 11, 9};
static const int RECPINS[3] =  {12, 10, 8};
static const int LEDPINS[3] =  {7, 5, 4};

static const uint8_t MOVAVE_CNT = 3;      // Used to calculate a moving ave (needs to be > 0)
static const uint16_t TOUCHSENSETHRESHOLD[3] = {80, 80, 80};     // Can adjust threshold sensitivity for each sensor

static long totalVal[3] = {0L};
static int totalnow[3] = {0};
static int totalprev[3] = {0};
static int totalsmooth[3][MOVAVE_CNT];
static uint8_t cntr[3] = {0};
static bool MAcalc[3] = {false};

static uint8_t LEDstate[3] = {0};

// This class will provide the PIO memory offset for the capsensing PIO (only need once)
PicoPIO capPicoPIO(pio0);
// Define each capacitive sensor (maximum per PIO is based on max number of State Machines available)
PicoCapSensing CapSensor1(capPicoPIO, TRIGPINS[0], RECPINS[0]);
PicoCapSensing CapSensor2(capPicoPIO, TRIGPINS[1], RECPINS[1]);
PicoCapSensing CapSensor3(capPicoPIO, TRIGPINS[2], RECPINS[2]);


void setup() {
  // put your setup code here, to run once:
  // initialise the LED Pin directions as output
  for (uint8_t i = 0; i < 3; i++) pinMode(LEDPINS[i], OUTPUT);

  Serial.begin(115200);
  //while(!Serial) {;;}

  Serial.println((CapSensor1.checkForError() < 0) ? "Capsensor 1: Setup Error": "Capsensor 1: Setup OK");
  Serial.println((CapSensor2.checkForError() < 0) ? "Capsensor 2: Setup Error": "Capsensor 2: Setup OK");
  Serial.println((CapSensor3.checkForError() < 0) ? "Capsensor 3: Setup Error": "Capsensor 3: Setup OK");

}


void loop() {
  totalnow[0] = CapSensor1.getCapSensingSample(1000, 30);
  totalnow[1] = CapSensor2.getCapSensingSample(1000, 30);
  totalnow[2] = CapSensor3.getCapSensingSample(1000, 30);

  // Formatted for the Serial Plotter
  Serial.print("0, ");

  for (uint8_t i = 0; i < 3; i++) {
    if (totalnow[i]) {
      if (totalprev[i]) {
        if (cntr[i] < MOVAVE_CNT) {
          if (abs(totalnow[i] - totalprev[i]) < TOUCHSENSETHRESHOLD[i]) {
            if (totalnow[i] > TOUCHSENSETHRESHOLD[i] || totalprev[i] > TOUCHSENSETHRESHOLD[i]) {
              if (totalnow[i] > totalprev[i]) totalsmooth[i][cntr[i]] = totalnow[i];
              else  totalsmooth[i][cntr[i]] = totalprev[i];
            }
            else totalsmooth[i][cntr[i]] = abs(totalnow[i] - totalprev[i]);
          }
          else totalsmooth[i][cntr[i]] = abs(totalnow[i] - totalprev[i]);
          cntr[i]++;
        }
        else {
          cntr[i] = 0;
          if (!MAcalc[i]) MAcalc[i] = true;
        }
        if (MAcalc[i]) {
          totalVal[i] = 0;
          for (uint8_t j = 0; j < MOVAVE_CNT; j++) {
            totalVal[i] += totalsmooth[i][j];
          }
          totalVal[i] /= MOVAVE_CNT;

          Serial.print("CAP"+String(i+1)+":"); Serial.print(totalVal[i]); Serial.print(", ");
          
          if (totalVal[i] > TOUCHSENSETHRESHOLD[i]) {
            if (!LEDstate[i]) {
              LEDstate[i] = !LEDstate[i];
              digitalWrite(LEDPINS[i], LEDstate[i]);
            }
          }
          else {
            if (LEDstate[i]) {
              LEDstate[i] = !LEDstate[i];
              digitalWrite(LEDPINS[i], LEDstate[i]);
            }
          }
        }
      }
      totalprev[i] = totalnow[i];
    }
  }
  Serial.println("2000");

  //Small delay for Serial Monitor output
  delay(50);

}
