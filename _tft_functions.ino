/**************************************************************************/
/*!
    @brief  Init TFT and Touchscreen
*/
/**************************************************************************/
void _tft_init()
{
  DBG_P(F("Init TFT... "));
  tft.reset();
  int ID = tft.readID();
  tft.begin(ID);
  tft.setRotation(0); // 0 = Portrait, 1 = Landscape
  tft.fillScreen(CLR_BLACK);
  tft.setTextWrap(false);
  DBG_P(F("Done.\n"));
}


/**************************************************************************/
/*!
    @brief  Draw default screen
*/
/**************************************************************************/
void _tft_drw_defaultscreen()
{
  char cStringTmp[10] = "";

  tft.fillRect(0, 140, 180, 180, CLR_DARKGRY);
  for (int iX = 0; iX <= 100; iX += 20) tft.drawFastVLine(log10(iX) * 30, 140, 300, CLR_LGHTGRY);
  for (int iX = 0; iX <= 100; iX += 20) tft.drawFastVLine((log10(iX) * 30) + 60, 140, 300, CLR_LGHTGRY);
  for (int iX = 0; iX <= 100; iX += 20) tft.drawFastVLine((log10(iX) * 30) + 120, 140, 300, CLR_LGHTGRY);
  _tft_drw_message(0, 130, 1, NULL, "0        10        1k        10k CPM", CLR_LGHTGRY, CLR_BLACK);
  tft.setRotation(1); // 0 = Portrait, 1 = Landscape
  _tft_drw_message(83, 90, 2, NULL, "CPM", CLR_LGHTGRY, CLR_BLACK);
  _tft_drw_cpm_window();
  _tft_drw_message(57, 150, 2, NULL, "nSv/h", CLR_LGHTGRY, CLR_BLACK);
  _tft_drw_message(25, 195, 2, NULL, sNuclideName, CLR_ORANGE, CLR_BLACK);
  _tft_drw_message(25, 215, 2, NULL, "H*(10)", CLR_ORANGE, CLR_BLACK);
  tft.setRotation(0); // 0 = Portrait, 1 = Landscape

}

/**************************************************************************/
/*!
    @brief  Draw CPM
    @param  value
    @param  length of string
*/
/**************************************************************************/
void _tft_drw_cpm(unsigned long val, unsigned int len)
{
  if (val >= 99999) {
    val = 99999;
  }
  tft.setFont(NULL);
  tft.setCursor(0, 57);
  tft.setTextColor(CLR_GREEN, CLR_BLACK);
  tft.setTextSize(4);
  int flen = 0, vi = abs((unsigned int)val);
  flen += vi >= 10000 ? 5 : vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
  for (int i = flen; i < len; ++i)
    tft.print(' ');
  tft.print(val);
}

/**************************************************************************/
/*!
    @brief  Draw CPM window
*/
/**************************************************************************/
void _tft_drw_cpm_window()
{
  char cStringTmp[5] = "";
  tft.setFont(NULL);
  tft.setCursor(20, 90);
  tft.setTextColor(CLR_LGHTGRY, CLR_BLACK);
  tft.setTextSize(2);
  tft.print("CPM (");
  tft.print(dtostrf((unsigned char)((DECAY_AVG_WINDOW - uiDecayArrayReducer) / 2), 1, 0, cStringTmp));
  tft.print("s)");
  if ((DECAY_AVG_WINDOW - uiDecayArrayReducer) < 10) tft.print(" ");
}


/**************************************************************************/
/*!
    @brief  Draw Sv/h
    @param  value (float)
    @param  length of string
    @param  precision of value
*/
/**************************************************************************/
void _tft_drw_svh(float val, unsigned int len, unsigned int prec)
{
  if (val >= 9999) {
    val = 9999;
  }
  tft.setFont(NULL);
  tft.setCursor(0, 117);
  tft.setTextColor(CLR_GREEN, CLR_BLACK);
  tft.setTextSize(4);
  unsigned int vi = abs((unsigned int)val);
  unsigned int flen = prec + (val < 0.0 ? 2 : 1); // . and -
  flen += vi >= 10000 ? 5 : vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
  for (int i = flen; i < len; ++i)
    tft.print(' ');
  tft.print(val, prec);
}


/**************************************************************************/
/*!
    @brief  Draw CPM graph
*/
/**************************************************************************/
void _tft_drw_graph()
{
  (uiTftVScroll > 0) ? uiTftVScroll-- : uiTftVScroll = 180;
  tft.vertScroll(140/*starting at x 140*/, 180/*for 180 pixels*/, uiTftVScroll/*shift n pixels*/);

  tft.drawFastHLine(0, 140 + uiTftVScroll, 240, CLR_BLACK);

  if ((uiTftVScroll % 10 == 0))
  {
    tft.setFont(NULL);
    tft.drawFastHLine(0, 140 + uiTftVScroll, 185, CLR_LGHTGRY);
    tft.setCursor(186, 140 + uiTftVScroll);
    tft.setTextColor(CLR_LGHTGRY, CLR_BLACK);
    tft.setTextSize(1);
    tft.print(fDecayCPM);
  }
  else
  {
    tft.drawFastHLine(0, 140 + uiTftVScroll, 180, CLR_DARKGRY);
  }

  if (fDecayCPM <= 10) {
    tft.drawFastHLine(0, 140 + uiTftVScroll, log10(fDecayCPM) * 30, CLR_DRKGRN);
  }
  else if (fDecayCPM <= 100) {
    tft.drawFastHLine(0, 140 + uiTftVScroll, (log10((fDecayCPM / 10)) * 30) + 60, CLR_DRKGRN);
  }
  else if (fDecayCPM <= 500) {
    tft.drawFastHLine(0, 140 + uiTftVScroll, (log10((fDecayCPM / 10)) * 30) + 60, CLR_GREEN);
  }
  else if (fDecayCPM <= 1000) {
    tft.drawFastHLine(0, 140 + uiTftVScroll, (log10((fDecayCPM / 10)) * 30) + 60, CLR_YELLOW);
  }
  else if (fDecayCPM <= 5000) {
    tft.drawFastHLine(0, 140 + uiTftVScroll, (log10((fDecayCPM / 100)) * 30) + 120, CLR_ORANGE);
  }
  else
  {
    tft.drawFastHLine(0, 140 + uiTftVScroll, (log10((fDecayCPM / 100)) * 30) + 120, CLR_RED);
  }
  for (int iX = 0; iX <= 100; iX += 20) tft.drawPixel(log10(iX) * 30, 140 + uiTftVScroll, CLR_LGHTGRY);
  for (int iX = 0; iX <= 100; iX += 20) tft.drawPixel((log10(iX) * 30) + 60, 140 + uiTftVScroll, CLR_LGHTGRY);
  for (int iX = 0; iX <= 100; iX += 20) tft.drawPixel((log10(iX) * 30) + 120, 140 + uiTftVScroll, CLR_LGHTGRY);
}



/**************************************************************************/
/*!
    @brief  Show message
    @param  X coordinate
    @param  Y coordinate
    @param  Text size
    @param  Font
    @param  Message
    @param  Font color
    @param  Background color
*/
/**************************************************************************/
void _tft_drw_message(int iX, int iY, int iSize, const GFXfont *cFont, const char *cMessage, const unsigned int iColor, const unsigned int iBgColor)
{
  tft.setFont(cFont);
  tft.setCursor(iX, iY);
  tft.setTextColor(iColor, iBgColor);
  tft.setTextSize(iSize);
  tft.print(cMessage);
}
