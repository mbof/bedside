# bedside

## Background

Bedside is an alarm clock and "morning companion". It is a connected
device, placed bedside, that fulfills the following jobs:
* Provide time of day and calendar information
* Make noise and lights to wake the user up
* Help with a morning routine by providing information such as weather
  forecast

The target user interface is:
* A relatively low-resolution LED matrix (32x32)
* Voice command for setting the time and alarm (mic + activation button)
* A speaker

## Current capabilities

The current version:
* Displays the time
* Displays upcoming weather temperatures
* Wakes up every day at a fixed time of day (e.g. every day at 7am)

It uses the project at rpi-rgb-led-matrix to provide the interface to the LED
matrix drivers.

## Credits

The
[tiny font](https://fontstruct.com/fontstructions/show/1404325/cg-pixel-4x5-2)
included in this repository was contributed to the public domain by Ilmari
Karonen.
