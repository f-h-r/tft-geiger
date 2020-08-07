/************************************************************************
  tft-geiger - Graphical Geiger-Mueller-Tube visualization
  (c)2020 by Mario Zimmermann & Marian Neubert

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 3 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
************************************************************************/

// Read configuration
#include "config.h"


/**************************************************************************/
/*!
    @brief  Setup Routine
*/
/**************************************************************************/
void setup(void)
{
  // Set up Serial if needed by debug functions
#ifdef DBG_ENABLE
  Serial.begin(SERIAL_BAUD);
#endif

  DBG_P(F("Init...\n"));

  memset((int)iDecayArray, 0, sizeof(iDecayArray)); // zeroing out measurement array
  _isr_init(); // Init ISR
  _tft_init(); // Inti TFT
  _tft_drw_defaultscreen(); // Draw default screen

  DBG_P(F("Init done.\n"));


}

/**************************************************************************/
/*!
    @brief  Main loop
*/
/**************************************************************************/
void loop()
{
  // timer flag set
  if (bTimerInterruptMask & (unsigned char)(1 << TIMER_TFT))
  {
    // calculate dose rate in nSv/h
    fDoseRate = (fDecayCPM * fAequiDosis[bNuclideId]) * 60;

    // update screen
    tft.setRotation(1); // 0 = Portrait, 1 = Landscape
    _tft_drw_cpm((unsigned long)fDecayCPM, 5); // draw CPM
    _tft_drw_cpm_window(); // draw CPM window

    // draw dose rate of current nuclide
    if (fDoseRate < 1000) // switch between dose displays
    {
      // print value in nSv/h
      _tft_drw_svh(fDoseRate, 6, 0);
      ucTftUpdateFlag &= ~(1 << TFT_UPD_SVH_RG); // clear bit
    }
    else
    {
      // print value in uSv/h
      _tft_drw_svh(fDoseRate / 1000, 5, 2);
      ucTftUpdateFlag |= (1 << TFT_UPD_SVH_RG); // set bit
    }


    // toggle dose range display
    if (!(ucTftUpdateFlag & (unsigned char)(1 << TFT_UPD_SVH)) && (ucTftUpdateFlag & (unsigned char)(1 << TFT_UPD_SVH_RG)))
    {
      _tft_drw_message(57, 150, 2, NULL, "uSv/h", CLR_LGHTGRY, CLR_BLACK);
      ucTftUpdateFlag |= (1 << TFT_UPD_SVH); // set bit
    }
    else if ((ucTftUpdateFlag & (unsigned char)(1 << TFT_UPD_SVH)) && !(ucTftUpdateFlag & (unsigned char)(1 << TFT_UPD_SVH_RG)))
    {
      _tft_drw_message(57, 150, 2, NULL, "nSv/h", CLR_LGHTGRY, CLR_BLACK);
      ucTftUpdateFlag &= ~(1 << TFT_UPD_SVH); // clear bit
    }

    tft.setRotation(0); // 0 = Portrait, 1 = Landscape
    _tft_drw_graph(); // Draw graph

    // reset draw-tft flag
    bTimerInterruptMask &= ~(1 << TIMER_TFT);
  }

}
