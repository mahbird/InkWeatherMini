#include "drawfunctions.h"

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display(GxEPD2_DRIVER_CLASS(CS, DC, RST, BSY));

void drawStringdisplay(int x, int y, String text, alignX alignx, alignY aligny)
{
  int16_t x1, y1;
  uint16_t w, h;
  display.setTextWrap(false);
  display.getTextBounds(text, x, y, &x1, &y1, &w, &h);
  if (alignx == RIGHT)
  {
    x -= w;
  }
  if (alignx == CENTER)
  {
    x -= w / 2;
  }
  if (aligny == TOP)
  {
    y += h;
  }
  if (aligny == MID)
  {
    y += h / 2;
  }
  display.setCursor(x, y);
  display.print(text);
}

void drawString(int x, int y, String text, alignX alignx, alignY aligny)
{
  int16_t x1, y1;
  uint16_t w, h;
  h = u8g2Fonts.getFontAscent();
  w = u8g2Fonts.getUTF8Width(text.c_str());
  if (alignx == RIGHT)
  {
    x -= w;
  }
  if (alignx == CENTER)
  {
    x -= w / 2;
  }
  if (aligny == TOP)
  {
    y += h;
  }
  if (aligny == MID)
  {
    y += h / 2;
  }
  u8g2Fonts.setCursor(x, y);
  u8g2Fonts.print(text);
}

void drawSRicon(int x, int y, int r)
{ // r = radius of the sun, total width = r*4, height = r*3
  display.drawFastVLine(x + r * 2, y + 1 - r * 3, r / 2, black);
  display.drawFastHLine(x, y - r, r / 2, black);
  display.drawFastHLine(x + r * 3.5, y - r, r / 2, black);
  display.drawLine(x + r * 0.5858, y - r * 2.4142, x + r * 0.9393, y - r * 2.0607, black);
  display.drawLine(x + r * 3.0607, y - r * 2.0607, x + r * 3.4142, y - r * 2.4142, black);
  display.fillCircle(x + r * 2, y - r - 1, r, yellow);
  display.drawCircle(x + r * 2, y - r - 1, r, black);
  display.fillRect(x, y - r * 3 / 4, r * 4, r * 3 / 4, black);
}

void drawSSicon(int x, int y, int r)
{ // r = radius of the sun, total width = r*4, height = r*2
  display.drawFastVLine(x + r * 2, y + 1 - r * 2.5, r / 2, black);
  display.drawLine(x + r * 0.5858, y - r * 1.9142, x + r * 0.9393, y - r * 1.5607, black);
  display.drawLine(x + r * 3.0607, y - r * 1.5607, x + r * 3.4142, y - r * 1.9142, black);
  display.fillCircle(x + r * 2, y - r * 0.5 - 1, r, red);
  display.drawCircle(x + r * 2, y - r * 0.5 - 1, r, black);
  display.fillRect(x, y - r * 3 / 4, r * 4, r * 3 / 4, black);
  display.fillRect(x + 1, y - r * 3 / 4 + 1, r * 4 - 2, r * 3 / 4 - 2, white);
  display.fillRect(x, y, r * 4, r / 2, white);
}

void drawAQicon(int x, int y, int aq, alignY aligny)
{
  int aqhi;
  if (aligny == MID)
  {
    y += 13;
  }
  if (aligny == BOTTOM)
  {
    y += 26;
  }
  /*if (aq == 0) {drawString (x+8, y, "No AQ data", RIGHT, TOP);
  return;}*/
  if (aq < 4)
  {
    aqhi = 0;
  }
  else if (aq < 7)
  {
    aqhi = 1;
  }
  else if (aq < 8)
  {
    aqhi = 2;
  }
  else if (aq < 11)
  {
    aqhi = 3;
  }
  else
  {
    aqhi = 4;
  }
  display.drawBitmap(x, y, AQicon[aqhi], 64, 26, black);
  u8g2Fonts.setForegroundColor(black);
  u8g2Fonts.setFont(tinyfont);
  drawString(x + 8, y, String(aq), RIGHT, TOP);
  u8g2Fonts.setForegroundColor(black);
  // display.drawRect(x, y, 64, 26, black); //test position
}

void drawUVbar(int x, int y, int w, int h, int UVI, alignY aligny)
{
  if (aligny == MID)
  {
    y += h / 2 + 1;
  }
  if (aligny == BOTTOM)
  {
    y += h + 2;
  }
  int u;
  display.drawRoundRect(x, y, w + 2, h + 2, 2, black);
  if (UVI < 3)
  {
    u = 0;
  }
  else if (UVI < 6)
  {
    u = 1;
  }
  else if (UVI < 8)
  {
    u = 2;
  }
  else if (UVI < 11)
  {
    u = 3;
  }
  else
  {
    u = 4;
  }

  for (int i = 0; i < u + 1; i++)
  {
    display.fillRect(x + 1 + i * w / 5, y + 1, w / 5, h, black);
  }
}

void drawwarnicon(int x, int y, int max_number, alignX alignx)
{
  int xr;
  for (int i = 0; i < max_number; i++)
  {
    if (alignx == RIGHT)
    {
      xr = x - ((1 + i) * 34);
    } // *32 for no border,  *34 to leave some space
    if (alignx == LEFT)
    {
      xr = x + i * 34;
    }
    if (alignx == CENTER)
    {
      return;
    } // center is invalid for warnicon

    if (warnicon[0] == "")
    {
      break;
    }
    else if (warnicon[i] == "")
    {
      break;
    }
    else
    {
      display.fillRect(xr, y, 32, 32, white);
    }

    if (warnicon[i] == "WHOT")
    {
      display.drawBitmap(xr, y, gImage_WHOT_y, 32, 32, yellow);
      display.drawBitmap(xr, y, gImage_WHOT_r, 32, 32, red); // HOT WEATHER WARNING
    }
    else if (warnicon[i] == "WCOLD")
    {
      display.drawBitmap(xr, y, gImage_WCOLD_b, 32, 32, black); // COLD WEATHER WARNING
    }
    else if (warnicon[i] == "WRAINA")
    {
      display.drawBitmap(xr, y, gImage_WRAINA_y, 32, 32, yellow);
      display.drawBitmap(xr, y, gImage_WRAINA_b, 32, 32, black); // AMBER RAINSTORM
    }
    else if (warnicon[i] == "WRAINR")
    {
      display.drawBitmap(xr, y, gImage_WRAINR_r, 32, 32, red); // RED RAINSTORM
    }
    else if (warnicon[i] == "WRAINB")
    {
      display.drawBitmap(xr, y, gImage_WRAINB_b, 32, 32, black); // BLACK RAINSTORM
    }
    else if (warnicon[i] == "WTS")
    {
      display.drawBitmap(xr, y, gImage_WTS_y, 32, 32, yellow);
      display.drawBitmap(xr, y, gImage_WTS_b, 32, 32, black); // THUNDERSTORM
    }
    else if (warnicon[i] == "TC1")
    {
      display.drawBitmap(xr, y, gImage_TC1_b, 32, 32, black);
    } // TYPHOON NO 1
    else if (warnicon[i] == "TC3")
    {
      display.drawBitmap(xr, y, gImage_TC3_b, 32, 32, black);
    } // TYPHOON NO 3
    else if (warnicon[i] == "TC8NE")
    {
      display.drawBitmap(xr, y, gImage_TC8NE_b, 32, 32, black);
    } // TYPHOON NO 8NE
    else if (warnicon[i] == "TC8SE")
    {
      display.drawBitmap(xr, y, gImage_TC8SE_b, 32, 32, black);
    } // TYPHOON NO 8SE
    else if (warnicon[i] == "TC8NW")
    {
      display.drawBitmap(xr, y, gImage_TC8NW_b, 32, 32, black);
    } // TYPHOON NO 8NW
    else if (warnicon[i] == "TC8SW")
    {
      display.drawBitmap(xr, y, gImage_TC8SW_b, 32, 32, black);
    } // TYPHOON NO 8SW
    else if (warnicon[i] == "TC9")
    {
      display.drawBitmap(xr, y, gImage_TC9_b, 32, 32, black);
    } // TYPHOON NO 9
    else if (warnicon[i] == "TC10")
    {
      display.drawBitmap(xr, y, gImage_TC10_b, 32, 32, black);
    } // TYPHOON NO 10
    else if (warnicon[i] == "WL")
    {
      display.drawBitmap(xr, y, gImage_WL_b, 32, 32, black);
    } // Landslide
    else if (warnicon[i] == "WFIREY")
    {
      display.drawBitmap(xr, y, gImage_WFIREY_y, 32, 32, yellow);
      display.drawBitmap(xr, y, gImage_WFIREY_b, 32, 32, black); // Yellow fire
    }
    else if (warnicon[i] == "WFIRER")
    {
      display.drawBitmap(xr, y, gImage_WFIRER_r, 32, 32, red);
    } // Red fire
  }

  if (warnicon[max_number] != "")
  {
    if (alignx == RIGHT)
    {
      display.fillRect(x - 34 * max_number - 6 - 1, y + 1, 2, 6, black);
      display.fillRect(x - 34 * max_number - 6 - 3, y + 3, 6, 2, black);
    }

    if (alignx == LEFT)
    {
      display.fillRect(x + 34 * max_number + 6 - 1, y + 1, 2, 6, black);
      display.fillRect(x + 34 * max_number + 6 - 3, y + 3, 6, 2, black);
    }
  }
}

void drawWeathericon(int x, int y, int weathericon)
{
  if (weathericon > 10 && weathericon < 19)
  { // Draw night weather icon
    display.fillRoundRect(x - 2, y - 2, 52, 52, 5, black);
    display.drawBitmap(x, y, icon_y[weathericon], 48, 48, yellow);
#ifdef display_BW
    display.drawBitmap(x, y, icon_b[weathericon], 48, 48, white);
#elif defined display_BWR
    display.drawBitmap(x, y, icon_b[weathericon], 48, 48, white);
#else
    display.drawBitmap(x, y, icon_b[weathericon], 48, 48, black);
#endif
  }

  else if (weathericon > 24)
  {
    display.drawBitmap(x, y, icon_b[weathericon], 48, 48, black);

#ifdef display_BW
    display.drawBitmap(x, y, icon_y[weathericon], 48, 48, black);
#elif defined display_BWR
    display.drawBitmap(x, y, icon_y[weathericon], 48, 48, black);
#else
    display.drawBitmap(x, y, icon_y[weathericon], 48, 48, yellow);
#endif
  }

  else
  {
    display.drawBitmap(x, y, icon_y[weathericon], 48, 48, yellow);
    display.drawBitmap(x, y, icon_b[weathericon], 48, 48, black);
  }
}

void drawerrorscreen()
{
  String errormessage;
  switch (errorcode)
  {
  case 1:
    errormessage = "No Wifi";

    break;

  case 2:
    errormessage = "Fail to get time";

    break;

  case 3:

    errormessage = "No Weather data";

    break;

  case 4:
    errormessage = "LOW BATTERY";

    break;
  default:
    // Serial.println("No error");
    break;
  }

  // Serial.println("Displaying Error Screen");
  display.init(0, false);
  u8g2Fonts.begin(display);
  u8g2Fonts.setFontMode(1);
  u8g2Fonts.setFontDirection(0);
  u8g2Fonts.setFont(smallfont);
  display.setRotation(display_rotation);
  u8g2Fonts.setForegroundColor(black);
  u8g2Fonts.setBackgroundColor(white);
  display.setFullWindow();
  display.firstPage();
  do
  {

    drawString(display.width() / 2, display.height() / 4, errormessage, CENTER, MID);
    u8g2Fonts.setFont(u8g2_font_spleen8x16_mf);

    if (errorcount > maxretrytimes)
    {
      if (errorcode == 4)
      {
        drawString(display.width() / 2, display.height() * 3 / 4, "Please charge battery and reset later", CENTER, MID);
      }
      else
      {
        drawString(display.width() / 2, display.height() * 3 / 4, "Please reset or check settings", CENTER, MID);
      }
    }
    else
    {
      drawString(display.width() / 2, display.height() * 3 / 4, "Device will retry in 30 mins", CENTER, MID);
    }

    /*
    if (errorcount > maxretrytimes)
    {

    drawString(display.width()/2, display.height()/4, errormessage, CENTER, MID);
      u8g2Fonts.setFont(u8g2_font_spleen8x16_mf);

    if (errorcode ==4) {drawString(display.width()/2, display.height()*3/4, "Please charge battery and reset later", CENTER, MID);}
    else {

      drawString(display.width()/2, display.height()*3/4, "Please reset or check settings", CENTER, MID);}
    }

    else
    {drawString(display.width()/2, display.height()/2, errormessage, CENTER, MID);} */

  }

  while (display.nextPage());
  display.hibernate();
}

/* 21x8 icon

void drawbatteryicon (int x, int y, int ws, int hs, int l){
display.fillRect(x, y, 19*ws, 8*hs, black);
display.fillRect(x+ws, y+hs, 17*ws, 6*hs, white);
display.fillRect(x+19*ws, y+hs*2, ws*2, hs*4, black);
for(int i = 0; i<4; i++)
{    if (i == l) {
        break;
    }
  display.fillRect(x+ws*2+i*4, y+hs*2, ws*3, hs*4, black);

}
} */

// 7*17 icon
void drawbatteryicon(int x, int y, int ws, int hs, int l)
{
  display.fillRect(x, y, 15 * ws, 7 * hs, black);
  display.fillRect(x + ws, y + hs, 13 * ws, 5 * hs, white);
  display.fillRect(x + 15 * ws, y + hs * 2, ws * 2, hs * 3, black);
  for (int i = 0; i < 4; i++)
  {
    if (i == l)
    {
      break;
    }
    display.fillRect(x + ws * 2 + i * 3, y + hs * 2, ws * 2, hs * 3, black);
  }
}
