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

#include "PicoCapSense.h"

/*************************************************************************
 @brief Constructor
 @param trigPin: trigger pin
 @param echoPin: echo pin
 @param pio: pio number 0=pio0, 1 = pio1
*************************************************************************/
PicoCapSense::PicoCapSense(int trigPin, int recPin, PIO pio=pio0):
    _trigPin(trigPin), _recPin(recPin), _pio(pio)

{
	  // initialize this instance's internal error checking variable
	  _error = 1;

	  if (_pio == pio0 || _pio == pio1) {
      _offset = pio_add_program(_pio, &capsense_program);
      _sm = pio_claim_unused_sm(_pio, false);
      if (_sm >= 0) {
        capsense_program_init(_pio, _sm, _offset, _trigPin, _recPin);
      }
      else _error = -2;
	  }
	  else _error = -1;
}

// @brief Destructor
PicoCapSense::~PicoCapSense(){}

/*************************************************************************
 @brief Get the CapSense Cycle Counts (options sample or single)
 @param timeout_millis: this is to a timeout value for the counter
 @param samples: number of samples to read using PIO
 @return: total count from sampling
*************************************************************************/
long PicoCapSense::getCapSenseSample(uint32_t timeout_millis, uint8_t samples) {
  if (_error < 0) return -1;
  uint32_t timeout_counts = (uint32_t)((float)(timeout_millis * F_CPU)/1000.0);
  if ((samples+2) > 102 ) return 0;
  if (samples == 1) {
  if (getCapSenseCounts(timeout_counts) < 0) return -2;   // variable over timeout
    _sampleTot = _total;
  }
  else {
    // We'll discard the min and max outliers - so first reset the min/max values
    _minval = 0xFFFFFF;
    _maxval = 0;
    _sampleTot = 0;
    for (uint8_t i = 0; i < (samples+2); i++) {    // loop for samples parameter
      _total = 0;
      if (getCapSenseCounts(timeout_counts) < 0)  return -2;   // variable over timeout
      _sampleTot += _total;
      _minval = (_total < _minval) ? _total : _minval;
      _maxval = (_total > _maxval) ? _total : _maxval;
    }

    _sampleTot = _sampleTot - _minval - _maxval;
  }

  return(_sampleTot);
}

// @brief Get the CapSense Cycle Count from PIO
// @param timeout_counts: this is to a timeout value based on CPU cycles
// @return cycle count
long PicoCapSense::getCapSenseCounts(uint32_t timeout_counts) {

  uint32_t clock_cycles;
  pio_sm_clear_fifos(_pio, _sm);
  pio_sm_put_blocking(_pio, _sm, timeout_counts);         // This triggers the first PIO routine

  // read one data item from the FIFO
  // Note we substract from timeout_counts as this is used as the countdown start value in the PIO
  clock_cycles = timeout_counts - pio_sm_get_blocking(_pio, _sm);

  if (!clock_cycles) {
    return -1;
  } else {
    _total = clock_cycles/1000;   // divide by 1000 to reduce value size (empirically derived - seems to work well enough)
    return 1;
  }

}
