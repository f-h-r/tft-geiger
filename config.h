// Libraries
#include <ccspi.h>
#include <MCUFRIEND_kbv.h> // NOTE: You need to comment out line 483 "vertScroll(0, HEIGHT, 0);" in MCUFRIEND_kbv.cpp!
#include <string.h>
#include "utility/debug.h"
#include <TimerOne.h>
#include <Wire.h>


// Debugging routines
#define DBG_ENABLE false
#define SERIAL_BAUD 115200
#ifdef DBG_ENABLE
#define DBG_P(...) { Serial.print(__VA_ARGS__); }
#else
#define DBG_P(...) {}
#endif


// SBM-20 geiger counter
#define EXTINT1 18 // External interrupt pin  for SBM-20 geiger counter (falling)
#define DEADTIME 0.00019 // Deadtime of SBM-20 geiger tube in seconds (correction not implemented)
#define DECAY_AVG_WINDOW 42 // Maximum size of array for calculating average decays (at least 5!)

volatile unsigned int uiDecayCounter = 0; // interrupt-suitable counter for decays, increased by external interrupt
volatile unsigned int iDecayArray[DECAY_AVG_WINDOW] = {}; // Memory for averaging decays

volatile unsigned char ucDecayArrayReducer = DECAY_AVG_WINDOW - 5 ; // Reduce the virtual "size" of iDecayArray[]. Useful for narrowing down avg-window.
volatile unsigned long fDecayCPM = 0; // interrupt-suitable average decay counter
float fDoseRate = 0; // current average dose rate
unsigned char bNuclideId = 0; // default nuclide is 137Cs

const char sNuclideName[7][6] = {"137Cs", "131I ", "226Ra", "90Sr ", "40K  ", "60Co ", "210Po"};
const float fAequiDosis[7] = {0.057, 1, 0.05, 0.05, 1, 1, 0.001}; // h10 (mSv/h)/GBq in 1 m distance - "Umgebungs-Aequivalentdosis" in (nSv/h)/Bq


// Timer-Interrupt-specific
#define TIMER_INT_US 500000 // Triggers every us
#define TIMER_INT_LOOPS 10 // Number of loops
#define TIMER_TFT 7 // Toggle-Bit in iTimerInterruptMask signalling TFT update
#define MAX_TIMER_INTERVALL 600 // Maximum interval in seconds for counter

// global timer interrupt indicator and counter
volatile unsigned int iTimerInterrupt = 1; // Interrupt-able Counter for recurring actions (16bit)
volatile unsigned char bTimerInterruptMask = 15; // Interrupt-able Char for storing bit masks and counters


// TFT-specific
// Colors - see http://www.barth-dev.de/online/rgb565-color-picker/
#define CLR_BLACK   0x0000
#define CLR_BLUE    0x001F
#define CLR_RED     0xF800
#define CLR_ORANGE  0xFC00
#define CLR_DRKGRN  0x0400
#define CLR_GREEN   0x07E0
#define CLR_CYAN    0x07FF
#define CLR_MAGENTA 0xF81F
#define CLR_YELLOW  0xFFE0
#define CLR_WHITE   0xFFFF
#define CLR_LGHTGRY 0x8410
#define CLR_DARKGRY 0x18E3

#define TFT_UPD_CPM 0 // position of status bit at ucTftUpdateFlag for CPM range update
#define TFT_UPD_SVH_RG 1 // position of status bit at ucTftUpdateFlag for indicating Sv/h range (0=nSv/h, 1=uSv/h)
#define TFT_UPD_SVH 2 // position of status bit at ucTftUpdateFlag for xSv/h range update

const int TFT_XP = 8, TFT_XM = A2, TFT_YP = A3, TFT_YM = 9; // Pins for TFT 240x320 ID=0x9341
unsigned int uiTftVScroll = 180;  // hardware scrolling counter
unsigned char ucTftUpdateFlag = 0; // char for indication-bits, e.g. for updating screen elements

MCUFRIEND_kbv tft; // start instance of TFT
