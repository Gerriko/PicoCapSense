# PicoCapSense

## An Arduino Capacitive Sensing library for RP2040 (Raspberry Pi Pico) boards, using the PIO processor.

### Capacitive Sensing is used to detect touch based on the electrical capacitance of the human body.


This library is based on the measuring methods used in the Capacitive Sensing Library for 'duino / Wiring (sampling method differs)
You can view this library here https://github.com/PaulStoffregen/CapacitiveSensor

It now handles multiple capacitive sensing touchpads. It uses one PIO state machine (sm) per capacitive sensor - i.e. max is 3 per PIO.

## Single Cap Sensing Example layout
![Fritzing Single CapSensing Breadboard Image](/image/CapacitiveSensingExample.jpg)

## Multi Cap Sensing Example layout
![Fritzing Multi CapSensing Breadboard Image](/image/multiCapSensingExample.jpg)

