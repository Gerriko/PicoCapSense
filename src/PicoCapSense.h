/*
 Copyright (c) 2022 C Gerrish (BigG/Gerrikoio/Gerriko)
 A capacitive Sensing library for RP2040 based boards (using the PIO processor).
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

#ifndef RP2040_PICOCAPSENSE_H
#define RP2040_PICOCAPSENSE_H

#include <Arduino.h>
#include "hardware/pio.h"
#include "hardware/dma.h"
#include "hardware/clocks.h"

#include "PicoCapSense.pio.h"

// Default CPU speed (125MHz) for the Pico (used to calculate timeout in cycles)
#if defined(ARDUINO_ARCH_RP2040) && !defined(F_CPU)
#define F_CPU 125000000L
#endif

class PicoCapSense
{
public:

    /*************************************************************************
     @brief Constructor
     @param trigPin: trigger pin
     @param echoPin: echo pin
     @param pio: pio number 0=pio0, 1 = pio1
    *************************************************************************/
    PicoCapSense(int trigPin, int readPin, PIO pio);

    // @brief destructor
    virtual ~PicoCapSense();

    /*************************************************************************
     @brief Get the CapSense Cycle Counts (options sample or single)
     @param timeout_millis: this is to a timeout value for the counter
     @param samples: number of samples to read using PIO
     @return: total count from sampling
    *************************************************************************/
    long getCapSenseSample(uint32_t timeout_millis, uint8_t samples);

private:

    // @brief Get the CapSense Cycle Count from PIO
    // @param timeout_counts: this is to a timeout value based on CPU cycles
    // @return cycle count
    long getCapSenseCounts(uint32_t timeout_counts);

	  int _error;
    uint32_t _total;
    uint32_t _sampleTot;
    uint32_t _minval;
    uint32_t _maxval;

    int _trigPin;
    int _recPin;

    PIO _pio;         // selected pio
    uint _offset;     // selected PIO memory offset
    int _sm;          // selected state machine

};

#endif // RP2040_PICOCAPSENSE_H
