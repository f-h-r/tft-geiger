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

  // periodically increase timerInterrupt from 1 to MAX_TIMER_INTERVALL
  iTimerInterrupt > MAX_TIMER_INTERVALL ? iTimerInterrupt = 1 : iTimerInterrupt++;

  // Set bits
  bTimerInterruptMask |= (1 << TIMER_TFT);

  // Decay handling:
  // Shift values
  for (volatile unsigned char uiTmp = (DECAY_AVG_WINDOW - (ucDecayArrayReducer - 1)); uiTmp > 0; uiTmp--)
  {
    iDecayArray[uiTmp] = iDecayArray[uiTmp - 1];
  }

  // Write new decay value to position 0
  iDecayArray[0] = uiDecayCounter;

  // Calculate mean value
  for (volatile unsigned char uiTmp = 0; uiTmp < (DECAY_AVG_WINDOW - ucDecayArrayReducer); uiTmp++)
  {
    fDecayCPMAvgTmp = fDecayCPMAvgTmp + iDecayArray[uiTmp];
  }

  // reduce or increase window to calculate average CPM based on current value
  // useful to reacting on large changes in counts
  if (uiDecayCounter + (unsigned int) fDecayCPMAvgTmp > 50)
  {
    ucDecayArrayReducer = DECAY_AVG_WINDOW - 5;
  }
  else if (ucDecayArrayReducer > 2)
  {
    ucDecayArrayReducer--;
  }

  // Calculate CPM (currently no deadtime correction!)
  fDecayCPM = ((fDecayCPMAvgTmp / (DECAY_AVG_WINDOW - ucDecayArrayReducer)) * 120);
  // Limit to 10k CPM
  if (fDecayCPM > 10000) fDecayCPM = 10000;

  // remove last value from counter for floating window
  if (iDecayArray[(DECAY_AVG_WINDOW - (ucDecayArrayReducer - 1))] - uiDecayCounter < 0) {
    uiDecayCounter -= iDecayArray[(DECAY_AVG_WINDOW - (ucDecayArrayReducer - 1))];
  }
  else
  {
    // zeroing out decay counter
    uiDecayCounter = 0;
  }
}
