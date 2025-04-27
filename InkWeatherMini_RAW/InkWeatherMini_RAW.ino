/* 2024-2025 By mahbird @ Github*/

/*
The InkWeatherMini and all icons are created and drawn by mahbird on Github, written in Arduino IDE 2.3.6, based on information available from the Hong Kong Observatory.
The code is distributed for free on github. For personal use only. Please retain all original messages when posting on other sites.
You are free to modify this code as you like, but do not redistribute modified code and do not use them elsewhere.
Commerical use is not allowed.
Please note that this software fetches data from public API services, (see credits), it is possible that this software may not be compatiable if the API changed in the future.

本天氣站InkWeatherMini以及圖示均為mahbird基於香港天文台的免費資訊制作及繪畫，於Arduino IDE 2.3.6編寫，並於github免費發佈，僅供個人使用，
轉載需保留原訊息，不得用於其他地方，可按需要自行修改但不得作二次發佈，不得用於商業用途。
本程序資訊取自開放API，(詳細看CREDIT)，如未來API有變更，可能令本程序不相容


!!
U8g2_for_Adafruit_GFX is used in this project, but the font list is not as updated as the main u8g2
The spleen font family used is one of them.
You need to download the both the u8g2 and U8g2_for_Adafruit_GFX library, then replace the font list in between /* start font list 
 /* C++ compatible in U8g2_for_Adafruit_GFX/src/U8g2_for_Adafruit_GFX.h with the ones in u8g2/csrc/u8g2.h
Then replace everything but the #include "u8g2_fonts.h" in U8g2_for_Adafruit_GFX/src/u8g2_fonts.c  with u8g2/csrc/u8g2_fonts.c
Once you are done replacing the files, you can uninstall the main u8g2 library
If you find this too complicated, you may consider trying different fonts
!!

I had problems with generating chinese font for u8g2 at that time, so I ended up just creating a "custom" file with FontForge
that puts chinese characters into the different glyphs, for example 
display.print("xyz"); //will print 月日時 


專頁 (Github page): https://github.com/mahbird/InkWeatherMini


Special thanks to the developer for the libraries (and the associated libararies in the libraries) used in this project:
(And install them if you haven't, if you encounter problems while compiling, try the versions on or before April, 2025)
ArduinoJson by bblanchon: https://github.com/bblanchon/ArduinoJson
GxEPD2 by ZinggJM: https://github.com/ZinggJM/GxEPD2
(and GxEPD2_4G if you are using b/w display: https://github.com/ZinggJM/GxEPD2_4G)
!! u8g2 and U8g2_for_Adafruit_GFX by olikraus: https://github.com/olikraus/u8g2 and https://github.com/olikraus/U8g2_for_Adafruit_GFX !! See notes above
Adafruit_GFX by Adafruit: https://github.com/adafruit/Adafruit-GFX-Library

Fonts used:
Spleen by fcambus: https://github.com/fcambus/spleen
open-huninn-font: https://github.com/justfont/open-huninn-font

Font converter: https://rop.nl/truetype2gfx/

Data service provider:
Weather data: https://www.hko.gov.hk/en/weatherAPI/doc/files/HKO_Open_Data_API_Documentation.pdf (Version 1.12, Date : Nov, 2024)
AQI: https://www.aqhi.gov.hk/psi/dd/hk_aqhiforecast_tc.pdf

NTP: time.nist.gov, pool.ntp.org, stdtime.gov.hk, time.google.com



Note: Use the custom partition suppled / partition scheme "Minimal SPIFFS" for more code space and 

*/



#include "configurations.h"


// voltage related variables
#ifdef batterypin
static RTC_DATA_ATTR bool voltagevalid = false;
static RTC_DATA_ATTR bool firstvoltageread = false;
static int VBAT = 0;
#endif

int nextupdatetime; // sleep time variable

void setup()
{
  // Serial.begin(115200);
  // Serial.printf("Error code:%d - Errorcount:%d\n", errorcode, errorcount);
  if (LED_on)
  {
    pinMode(LED_pin, OUTPUT);
  }

#ifdef batterypin
  if (!firstvoltageread)
  {
    voltagevalid = true;
  }

  VBAT = readBATvoltage();
  // Serial.printf("Voltage: %dmV\n", VBAT);
  if (VBAT != 0 && VBAT < 3100) // low battery, ignore 0
  {
    errorcode = 4;
    // Serial.println("Low battery, sleeping indefinitely");
    errorcount = 99; // Will not be woken up by GPIO this effectively disables wifi related functions until battery is charged
    drawerrorscreen();
    delay(50);
    esp_deep_sleep_start();
  }
#endif

  wifiConnect();
  LEDlongflashstart();
  // Serial.printf("Finished wificonnect(), errorcode: %d, errorcount: %d\n", errorcode, errorcount);
  if (errorcode != 1)
  {
    wifiNTP();
    if (errorcode == 0)
    {
      for (int i = 0; i < 3; i++)
      {
        getCurrentWeather();
        getForecast();
        getSRS();
        getwarning();
        getVisibility();
        if (CurrentTmpC == -1 || CurrentRH == -1)
        {
          errorcode = 3;
        } // just in case, fail to get weather
        if (errorcode != 3)
        {
          // Serial.println("Get Weather successful");
          errorcode = 0; // just in case
          break;
        }
        // Serial.printf("Fail to get weather, attempt %d, retrying in 3s\n", i+1);
        delay(3000);
      }
      if (errorcode == 0)
      {
        for (int i = 0; i < 3; i++)
        {
          getAQI(); // On error, AQI appears as 0
          if (AQI != 0)
          {
            // Serial.println("Get AQI successful");
            errorcode = 0;
            break;
          }
          // Serial.printf("Fail to get AQI, attempt %d, retrying in 3s\n", i+1);
          // errorcode=3;
          delay(3000);
        }
      }
      LEDlongflashstop();

      // Serial.printf("Finished getting data, errorcode: %d, errorcount: %d\n", errorcode, errorcount);
    }
  }
  WiFi.mode(WIFI_OFF);
  // LEDWrite(LED_pin,HIGH);

  setCpuFrequencyMhz(40);

  if (errorcode == 0)
  {
    errorcount = 0;
    nextupdatetime = sleepminute();
    drawall();
  }

  else
  {
    errorcount++;
    // Serial.printf("Error, errorcode: %d, errorcount: %d\n", errorcode, errorcount);

    if (errorcount < 3)
    {
      // Serial.printf("Error count: %d \nSleeping for 5s\n", errorcount);
      delay(50);
      esp_deep_sleep(5e6); // retry in 5seconds
    }

    else if (errorcount == 3)
    {
      drawerrorscreen();
      // Serial.printf("Error count: %d \nSleeping for 30min, Error screen drawn\n", errorcount);
      delay(50);
      esp_deep_sleep(30 * 6e7);
    } // 30 mintute

    else if (errorcount <= maxretrytimes)
    {
      // Serial.printf("Error count: %d \nSleeping for 30min\n", errorcount);
      delay(50);
      esp_deep_sleep(30 * 6e7); // for maxretry = 5, this retries 2 more times (not counting the 3rd retry) 30min
    }
    else
    {
      drawerrorscreen();
      // Serial.printf("Error count: %d \nSleeping indefinitely\n", errorcount);
      delay(50);
      esp_deep_sleep_start();
    } // sleep indefinitely
  }

  esp_deep_sleep((nextupdatetime + 5) * 6e7); // add 5 min
}

void loop() {}

void drawall()
{
  // Serial.println("Displaying");
  display.init(0, true); // true for normal full refresh and false for fast refresh, recommended full refresh for cleaner screen
  u8g2Fonts.begin(display);
  u8g2Fonts.setFontMode(1);      // use u8g2 transparent mode (this is default)
  u8g2Fonts.setFontDirection(0); // left to right (this is default)
  display.setRotation(display_rotation);
  u8g2Fonts.setForegroundColor(black);
  u8g2Fonts.setBackgroundColor(white);
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(white);

    // Draw Weather
    int x = 2;
    int y = 0;

    int th = 15; // chinese font height

    // line 1
    display.setFont(&tc_huninn7pt7b); // Chinese font - set as default GFX font, so use with display.print. Use u8g2.print for u8g2 fonts.
    display.setTextColor(black);

    char cwline1[20];
    String ampm = "mo"; // 上午
    if (nowhour > 17)
    {
      ampm = "qm";
    } // 晚上
    else if (nowhour > 11)
    {
      ampm = "no";
    } // 下午
    int printnowhour = nowhour;
    if (nowhour > 12)
    {
      printnowhour -= 12;
    }

    display.setCursor(x, y + th);
    display.printf("%dx%dy%s%dz", nowmonth, nowday, ampm, printnowhour); // d月d日s午d時
    u8g2Fonts.setFont(smallfont);
    char cwline2[20];
    snprintf(cwline2, sizeof(cwline2), "%d°C|%d%%", CurrentTmpC, CurrentRH);
    drawString(x, y + th + 25, cwline2, LEFT, MID);

    // Vis

    // draw sunset sunrise SRS
    u8g2Fonts.setFont(tinyfont);
    drawSRicon(x + 2, y + 80, 7);
    drawSSicon(x + 2 + 40, y + 80, 7);
    drawString(x + 2 + 7 * 2, y + 82, sunrise, CENTER, TOP);
    drawString(x + 2 + 40 + 7 * 2, y + 82, sunset, CENTER, TOP);
    char uvtext[6];
    snprintf(uvtext, sizeof(uvtext), "UV:%d", uv);
    drawString(x + 85 + 16, y + 76 - 2, uvtext, CENTER, BOTTOM);
    drawUVbar(x + 85, y + 76, 30, 8, uv, TOP);
    // drawAQicon (x, display.height()-26-2, AQI, TOP); // icon is 64 width
    // drawwarnicon(x+66, display.height()-32-2, LEFT); // align warnicon to left of AQicon
    drawAQicon(x + 39, display.height() - 26, AQI, TOP);

    /*
   char dayofweek[3];
   char wday_tc[7] = {'y', 'a', 'b', 'c', 'd', 'e', 'f'};
   display.setCursor(x, display.height()-th);
   display.printf("st%c", wday_tc[nowwday]); */
    // Day of week in chinese, not used in the final version but leaving it in case you'd like to try. Will overlap with others

display.drawBitmap(x, display.height()-19, gImage_panda_right_37x19, 37, 19, black); //panda icon


    // draw panda zzz if sleep time
    if (nextupdatetime > updateinterval * 60)
    {
      display.setFont();
      display.setTextSize(1);
      display.setCursor(x, display.height() - 28);
      display.print("z Z Z");
    }
    // day of week in circle
    else
    {
      for (int i = 0; i < 7; i++)
      {
        if (nowwday == 0)
        {
          nowwday += 7;
        }
        if (i < nowwday)
        {
          display.fillRoundRect(x + 1 + i * 5, display.height() - 26, 4, 4, 2, black);
        }
        else
        {
          display.drawPixel(x + 2 + i * 5, display.height() - 25, black);
          // display.fillCircle(x+2+i*5, display.height()-25, 2, black);
        }
      }
    }
    // current weather icon
    int x1 = x + 140; // position of the current weather icon
    int y1 = y + 5;
    int xtxt = x1 - 48;

    // draw warnicon, if more than 3 warnicon, draw + sign, align with current weathericon

    drawwarnicon(x1 + 50, display.height() - 32, 2, RIGHT); // x1+48 = align to right of currentweathericon

    drawWeathericon(x1, y1, weathericon); // also adds a 2pt bordered round Rect for the night weather

    u8g2Fonts.setFont(tinyfont);
    // min max tmpC today
    char todayminmaxtmpC[10];
    snprintf(todayminmaxtmpC, sizeof(todayminmaxtmpC), "%d-%d°C", todaymintmpC, todaymaxtmpC);
    drawString(x1 + 48, y1 + 54, todayminmaxtmpC, RIGHT, TOP); // align to right
    // drawString(x1, y1+55, todayminmaxtmpC, LEFT, TOP); // align to current weathericon
    char todayminmaxRH[12];
    snprintf(todayminmaxRH, sizeof(todayminmaxRH), "%d-%d%%", todayminRH, todaymaxRH);
    drawString(x1 + 48, y1 + 66, todayminmaxRH, RIGHT, TOP); // align to right
    char rainfall[12];
    u8g2Fonts.setForegroundColor(black);
    if (rainmax > 0)
    {
      snprintf(rainfall, sizeof(todayminmaxtmpC), "%d-%dmm", rainmin, rainmax);
      drawString(x1 + 48, y1 + 78, rainfall, RIGHT, TOP);
    }
    else
    {
      String PSR;
      if (todayrainchance == "High")
      {
        PSR = ">70%";
      }
      else if (todayrainchance == "Medium High")
      {
        PSR = "55-69%";
      }
      else if (todayrainchance == "Medium")
      {
        PSR = "45-54%";
      }
      else if (todayrainchance == "Medium Low")
      {
        PSR = "30-44%";
      }
      else
      {
        PSR = "<30%";
      }
      int wr;
      int hr;
      hr = u8g2Fonts.getFontAscent();
      wr = u8g2Fonts.getUTF8Width(PSR.c_str());
      u8g2Fonts.setCursor(x1 + 48 - wr, y1 + 66 + hr + 12);
      u8g2Fonts.print(PSR);
      // u8g2Fonts.setCursor(x1+11, y1+70+10);
      // u8g2Fonts.print(PSR);
      // drawString (x2+11, y+i*bh+th+rh*2, PSR, LEFT, TOP);
      display.drawBitmap(x1 + 48 - wr - 11, y1 + 66 + 12 - 1, gImage_PSR_9x10, 9, 10, black);
    }

    // draw forecast

    u8g2Fonts.setForegroundColor(black);

    int x2 = x1 + 58;
    int bh = 64; // box height for each forecast day
    int rh = 14; // row height for each forecast day

    display.fillRect((x2 + x1 + 48) / 2, y, 1, display.height(), black);

    // draw forecasticon

    // 明日後日
    display.setFont(&tc_huninn7pt7b);
    display.setCursor(x2 - 2, y + th);
    display.print("uy");
    display.setCursor(x2 - 2, y + bh + th);
    display.print("vy");

    th = th + 6;
    for (int i = 0; i < 2; i++)
    {
      u8g2Fonts.setFont(tinyfont);
      char fcline1[15];
      snprintf(fcline1, sizeof(fcline1), "%d-%d°C", forecastminTmpC[i], forecastmaxTmpC[i]);
      char fcline2[15];
      snprintf(fcline2, sizeof(fcline2), "%d-%d%%", forecastminRH[i], forecastmaxRH[i]);
      drawString(x2, y + i * bh + th + 2, fcline1, LEFT, TOP);
      u8g2Fonts.setForegroundColor(black);
      drawString(x2, y + i * bh + th + rh + 2, fcline2, LEFT, TOP);
      u8g2Fonts.setForegroundColor(black);
      String PSR;
      if (forecastPSR[i] == "High")
      {
        PSR = ">70%";
      }
      else if (forecastPSR[i] == "Medium High")
      {
        PSR = "55-69%";
      }
      else if (forecastPSR[i] == "Medium")
      {
        PSR = "45-54%";
      }
      else if (forecastPSR[i] == "Medium Low")
      {
        PSR = "30-44%";
      }
      else
      {
        PSR = "<30%";
      }
      // u8g2Fonts.setCursor(x2+11, y+i*bh+th+rh*3-6);
      drawString(x2 + 11, y + i * bh + th + rh * 2 + 2, PSR, LEFT, TOP);
      display.drawBitmap(x2, y + i * bh + th + rh * 2 + 1, gImage_PSR_9x10, 9, 10, black);
      // u8g2Fonts.print(PSR);
      // display.drawBitmap(x2, y+i*bh+th+rh*3-4-10-1, gImage_PSR_9x10, 9, 10, black);

      // display.drawBitmap(display.width()-50,y+i*bh+(bh-48)/2, icon_y[forecasticon[i]], 48, 48, white); // align to screen right , center of the box
      // display.drawBitmap(display.width()-50, y+i*bh+(bh-48)/2, icon_b[forecasticon[i]], 48, 48, black);
      display.drawBitmap(display.width() - 48, y + i * bh + 5, icon_y[forecasticon[i]], 48, 48, white); // align to screen right //
      display.drawBitmap(display.width() - 48, y + i * bh + 5, icon_b[forecasticon[i]], 48, 48, black);
      if (i > 0)
      {
        display.fillRect((x2 + x1 + 48) / 2, y + i * bh, display.width() - ((x2 + x1 + 48) / 2), 1, black);
      }
    }

// draw battery
#ifdef batterypin
    if (voltagevalid)
    {
      int Vlvl = voltagetolevel(VBAT);
      drawbatteryicon(display.width() - 17, display.height() - 7, 1, 1, Vlvl);
    }

    // Displays raw values
    // if (VBAT < 3.6) {drawString(display.width(),display.height(), "Battery Low", RIGHT, BOTTOM);}
    // u8g2Fonts.setFont(u8g2_font_tiny5_tr);
    // drawString(display.width()-20,display.height(), String((float)VBAT/1000, 2) +"V", RIGHT, BOTTOM);

#endif

  } while (display.nextPage());
  // Serial.println("Draw all done");
  display.hibernate();
}

int sleepminute()
{
  int sleeptime = 0;
  if (sleephour == wakehour) // disable sleeps if they are the same
  {
    sleeptime = updateinterval * 60 - nowminute;
  }
  else if (sleephour > wakehour)
  {
    if (nowhour >= sleephour && nowhour < 24)
    {
      sleeptime = (24 - nowhour + wakehour) * 60 - nowminute;
    }
    else if (nowhour < wakehour)
    {
      sleeptime = (wakehour - nowhour) * 60 - nowminute;
    }
    else
    {
      sleeptime = updateinterval * 60 - nowminute;
    }
  }

  else if (sleephour < wakehour)
  {
    if (nowhour >= sleephour && nowhour < wakehour)
    {
      sleeptime = (wakehour - nowhour) * 60 - nowminute;
    }
    else
    {
      sleeptime = updateinterval * 60 - nowminute;
    }
  }
  if (nowhour == wakehour - 1)
  {
    esp_deep_sleep((updateinterval * 60 - nowminute + 5) * 6e7);
  } // in case it wakes up too early, goes back to sleep until it is wakehour (adds 5min)

  return sleeptime;
}

#ifdef batterypin
int readBATvoltage()
{
  if (voltagevalid)
  {
    int BATvoltage = 0; //
    int VBATtemp;
    int count = 0;
    pinMode(batterypin, INPUT);
    analogReadResolution(12);
    for (int i = 0; i < 3; i++)
    {
      VBATtemp = analogReadMilliVolts(batterypin) * 2;
      // Serial.println(VBATtemp);
      if (VBATtemp < 4800 && VBATtemp > 3000)
      { // omit irregular reading
        BATvoltage += VBATtemp;
        count++;
      }
      delay(100);
    }
    if (count > 0)
    {
      BATvoltage /= count;
    }
    else
    {
      voltagevalid = false;
      return 0;
    }
    if (BATvoltage < 4800 && BATvoltage > 3000)
    {
      return BATvoltage;
    }
    else
    {
      voltagevalid = false;
      return 0;
    }
  }
  else
  {
    voltagevalid = false;
    return 0;
  }
}

int voltagetolevel(int voltage)
{
  int voltage100 = 4000;
  int voltage75 = 3869;
  int voltage50 = 3799;
  int voltage25 = 3629;
  if (voltage > voltage100)
  {
    return 4;
  }
  else if (voltage > voltage75)
  {
    return 3;
  }
  else if (voltage > voltage50)
  {
    return 2;
  }
  else if (voltage > voltage25)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

#endif
