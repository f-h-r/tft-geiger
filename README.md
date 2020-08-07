# Project tft-geiger for Arduino
A software for Arduino to display radioactivity measured from the SBM-20 driver in a graphical manner.

## Features
* Quick measurement of Counts per Minute (CPM) and dose rate in nano- and micro Sievert per hour
* Autoscaling buffer for CPM, depending on decays registered
* selectable isotope for dose rate, Cs137 preselected
* scrolling CPM-histogramm with logarithmic display and color indication - refreshed every 0,5s
* *Important Note:* The displayed dose rate is just for a rough estimation of radiation based on Hp(10). SBM-20 and RD003 tubes are not energy compensated or calibrated!
* *Important Note 2:* You need to comment out line 483 "vertScroll(0, HEIGHT, 0);" in MCUFRIEND_kbv.cpp, so that the drawing while scrolling works!

## Components needed
* 1x Arduino Mega2560 (Arduino Uno works also but a change to your used pins/ports is needed in the code)
* 1x 2,8" (Touch-)TFT 240x320 (ID=0x9341) with 8-Bit-Interface
* 1x SBM20-Driver for Geiger-Mueller-Tubes (using 1 interrupt-capable input-pin)

## Libraries used
* ccspi
* MCUFRIEND_kbv
* string
* utility/debug
* TimerOne
* Wire

## Ports and Pins used
### Arduino Mega 2560
Pin  | Usage
-----|-----------
 18  | external Interrupt for SBM-20-Driver

### TFT shield
Pin  | Usage
-----|-----------
  1  | GND
  2  | 5V
  3  | -
  4  | RS (Arduino-PIN 22)
  5  | R/W (Arduino-PIN 24)
  6  | EN (Arduino-PIN 26)
  7  | D0 (Arduino-PIN 28)
  8  | D1 (Arduino-PIN 30)
  9  | D2 (Arduino-PIN 32)
 10  | D3 (Arduino-PIN 34)
 11  | D4 (Arduino-PIN 36)
 12  | D5 (Arduino-PIN 38)
 13  | D6 (Arduino-PIN 40)
 14  | D7 (Arduino-PIN 42)
 15  | -
 16  | -
 17  | -
 18  | -
 19  | -
 20  | LED-K/FET-Drain (Arduino-PIN 44)
