/**************************************************************************/
/*!
    @brief  Init interruptouchscreen
*/
/**************************************************************************/
void _isr_init()
{
  // external interrupt for geiger counter
  DBG_P(F("Init Interrupt... "));
  pinMode(EXTINT1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(EXTINT1), _isr_decay, FALLING);
  DBG_P(F("Done.\n"));

  // Initialize Timer, attach interrupt routine
  DBG_P(F("Init Timer... "));
  Timer1.initialize(TIMER_INT_US); // every n microseconds
  Timer1.attachInterrupt(_isr_timer);
  DBG_P(F("Done.\n"));
}

/**************************************************************************/
/*!
    @brief  Called on every decay counted by geiger counter
*/
/**************************************************************************/
void _isr_decay()
{
  // Increase decay counter
  uiDecayCounter++;
}


/**************************************************************************/
/*!
    @brief  Function is called on overflow of Timer1
*/
/**************************************************************************/
void _isr_timer()
{
  volatile float fDecayCPMAvgTmp = 0; // Temporary counter for averaging
  volatile unsigned int uiDecayCounterTmp = uiDecayCounter; // Temporary storage for current decay value
  interrupts(); // enable nested interrupts

  // Decay handling. Shift values
  for (volatile unsigned int uiTmp = (DECAY_AVG_WINDOW - (uiDecayArrayReducer - 1)); uiTmp > 0; uiTmp--)
  {
    uiDecayArray[uiTmp] = uiDecayArray[uiTmp - 1];
  }

  // Write new current decay value to position 0
  // with dead time correction for each tube -> ACTUAL COUNTS = OBSERVED COUNTS / (1 - (OBSERVED COUNTS * DEADTIME))
  uiDecayArray[0] = (unsigned int)((float)uiDecayCounterTmp / (1 - ((float)uiDecayCounterTmp * DEADTIME)));

  // Calculate mean value
  for (volatile unsigned int uiTmp = 0; uiTmp < (DECAY_AVG_WINDOW - uiDecayArrayReducer); uiTmp++)
  {
    fDecayCPMAvgTmp = fDecayCPMAvgTmp + uiDecayArray[uiTmp];
  }

  // reduce or increase window to calculate average CPM based on current value
  // useful to reacting on large changes in counts
  if (uiDecayCounterTmp + (unsigned int) fDecayCPMAvgTmp > 50)
  {
    uiDecayArrayReducer = DECAY_AVG_WINDOW - 5;
  }
  else if ((uiDecayArrayReducer > 2) && (uiDecayArrayReducer > uiDecayArrayReducerSize))
  {
    uiDecayArrayReducer--;
  }

  // Calculate CPM
  fDecayCPM = ((fDecayCPMAvgTmp / (DECAY_AVG_WINDOW - uiDecayArrayReducer)) * 60);

  // Limit to 10k CPM
  if (fDecayCPM > 10000) fDecayCPM = 10000;

  // remove last value from decay counter for floating window
  if (uiDecayArray[(DECAY_AVG_WINDOW - (uiDecayArrayReducer - 1))] - uiDecayCounterTmp < 0) {
    noInterrupts();
    uiDecayCounter -= uiDecayArray[(DECAY_AVG_WINDOW - (uiDecayArrayReducer - 1))];
    interrupts();
  }
  else
  {
    // or zeroing out decay counter
    noInterrupts();
    uiDecayCounter = 0;
    interrupts();
  }

  // Set bits
  ucTimerInterruptMask |= (1 << TIMER_TFT);
}
