# Randuino project
The randuino project is about creating a truly non-deteministic random number generator using an arduino board and the given software.

## Arduino scripts
The `arduino-scripts` folder contains scripts for multiple kinds of randomizers.
* `randuino-timing.ino` works by picking a random number depending on the point in time when a digital signal arrives.
* `randuino-value.ino` works by picking a random number depending on the current on a analog port.

## Randuino library
The randuino library helps communicationg to a randuino. 
It is written in C++ and Qt 5.
