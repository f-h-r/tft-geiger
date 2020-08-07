# Project tft-geiger for Arduino
A software for Arduino to display radioactivity measured from the SBM-20 driver in a graphical manner.

## Features
* Quick measurement of Counts per Minute (CPM) and dose rate in nano- and micro Sievert per hour
* Autoscaling buffer for CPM, depending on decays registered
* selectable isotope for dose rate, Cs137 preselected
* scrolling CPM-histogramm with logarithmic display and color indication - refreshed every 0,5s
* *Important Note:* The displayed dose rate is just for a rough estimation of radiation based on Hp(10). SBM-20 and RD003 tubes are not energy compensated or calibrated!

## Components needed
* 1x Arduino Mega2560 (Arduino Uno works also but a change to your used pins/ports is needed in the code)
* 1x 2,8" (Touch-)TFT 240x320 (ID=0x9341) with 8-Bit-Interface
* 1x SBM20-Driver for Geiger-Mueller-Tubes (using 1 interrupt-capable input-pin)

