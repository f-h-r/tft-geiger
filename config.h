// Libraries
#include <SPI.h>
#include <MCUFRIEND_kbv.h> // NOTE: You need to comment out line 483 "vertScroll(0, HEIGHT, 0);" in MCUFRIEND_kbv.cpp!
#include <string.h>
#include <TimerOne.h>


// Debugging routines
#define DBG_ENABLE false
#define SERIAL_BAUD 115200
#ifdef DBG_ENABLE
#define DBG_P(...) { Serial.print(__VA_ARGS__); }
#else
#define DBG_P(...) {}
#endif


// driver for Geiger-Mueller-Tubes
#define EXTINT1 18 // External interrupt pin for driver(falling)
//#define DEADTIME 0.00019 // Deadtime of geiger tube in seconds (190uS, SBM-20)
#define DEADTIME 0.000075 // Deadtime of geiger tube in seconds (75uS, RD003/KB6011)
#define DECAY_AVG_WINDOW 42 // Maximum size of array for calculating average decays (at least 5!)

volatile unsigned int uiDecayCounter = 0; // interrupt-suitable counter for decays, increased by external interrupt
volatile unsigned int uiDecayArray[DECAY_AVG_WINDOW] = {0}; // Memory for averaging decays
volatile unsigned int uiDecayArrayReducer = DECAY_AVG_WINDOW - 5 ; // Reduce the virtual "size" of uiDecayArray[]. Useful for narrowing down avg-window.
volatile unsigned int uiDecayArrayReducerSize = 2 ; // window to check of uiDecayArray[]
volatile float fDecayCPM = 0; // interrupt-suitable average decay counter
float fDoseRate = 0; // current average dose rate

const char sNuclideName[6] = {"226Ra"};
//const float fEquivDose = 290; // 29 Imp/s / mR/h = 290 Imp/s / uSv/h (equivalent dose h*10 Ra226) - SBM-20
const float fEquivDose = 420; // 42 Imp/s / mR/h = 420 Imp/s / uSv/h (equivalent dose h*10 Ra226) - RD003/KB6011


// Timer-Interrupt-specific
#define TIMER_INT_US 500000 // Triggers every us
#define TIMER_INT_LOOPS 10 // Number of loops
#define TIMER_TFT 7 // Toggle-Bit in ucTimerInterruptMask signalling TFT update

// global timer interrupt indicator and counter
volatile unsigned char ucTimerInterruptMask = 0; // Interrupt-able Char for storing bit masks and counters


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
