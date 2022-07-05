/*
 Copyright (c) 2022 C Gerrish (BigG/Gerrikoio/Gerriko)
 This example takes a rolling average to smooth out capacitance sampled values.

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

 #include "PicoCapSense.h"

// define the pin numbers:
static const int TRIGPIN =  13;
static const int RECPIN =  12;
static const int LEDPIN = 20;

static const uint8_t MOVAVE_CNT = 3;      // Used to calculate a moving ave (needs to be > 0)

static long totalVal = 0L;
static int totalnow = 0;
static int totalprev = 0;
static int totalsmooth[6];
static uint8_t cntr = 0;
static bool MAcalc = false;

static uint8_t LEDstate = 0;

PIO pio = pio0;

PicoCapSense CapSense(TRIGPIN, RECPIN, pio0);

void setup() {
  // put your setup code here, to run once:
  pinMode(LEDPIN, OUTPUT);

  Serial.begin(115200);
  while(!Serial) {;;}

}


void loop() {
  totalnow = CapSense.getCapSenseSample(2000, 30);
  if (totalnow) {
    if (totalprev) {
      if (cntr < MOVAVE_CNT) {
        if (abs(totalnow - totalprev) < 150) {
          if (totalnow > 150 || totalprev > 150) {
            if (totalnow > totalprev) totalsmooth[cntr] = totalnow;
            else  totalsmooth[cntr] = totalprev;
          }
          else totalsmooth[cntr] = abs(totalnow - totalprev);
        }
        else totalsmooth[cntr] = abs(totalnow - totalprev);
        cntr++;
      }
      else {
        cntr = 0;
        if (!MAcalc) MAcalc = true;
      }
      if (MAcalc) {
        totalVal = 0;
        for (uint8_t i = 0; i < MOVAVE_CNT; i++) {
          totalVal += totalsmooth[i];
        }
        totalVal /= MOVAVE_CNT;
        Serial.print("0, ");              // sets the min value on y-axis
        Serial.print(totalVal);           // print smoothed output from capsensor output
        Serial.println(", 2000");           // sets the max value on y-axis (although it can shift)

        if (totalVal > 150) {
          if (!LEDstate) {
            LEDstate = !LEDstate;
            digitalWrite(LEDPIN, LEDstate);
          }
        }
        else {
          if (LEDstate) {
            LEDstate = !LEDstate;
            digitalWrite(LEDPIN, LEDstate);
          }
        }
      }
    }
    totalprev = totalnow;
  }
  delay(50);                             // arbitrary delay to limit data to serial port

}
