# Packager-Ctrl

Arduino Due application for packaging machine automation.

## Description

Arduino Due program to control all functions from a packaging machine as a substitute for the common CLP/uCLP controller.

This machine has 3 buttons and 3 indicator leds on the panel, 3 security sensors for doors (active all the time) and 3 operation sensors (Photocell, End of cycle and Knife releaser).

Operation is simple, each of the 3 buttons turns something ON\OFF, press it once to turn ON and again to turn OFF. The first button control the machine production, the second button controls the feeder and the last one controls the dater.

Repetitive functions are controlled via a cycle timer, auto-adjusting to fit inside the cycle based on the last cycle duration. It could easily be modified to work with an Encoder.

To add more timed functions, just make the new function struct on Config.h and schedule it where you want it.
