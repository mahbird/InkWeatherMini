#ifndef configurations_h
#define configurations_h

// common libraries
#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <Preferences.h>
#include <ArduinoJson.h>

// Headers
#include "HKOWeatherJSON.h"
#include "drawfunctions.h"

extern Preferences pref;

// For JSON weather

// time
extern int timezone, daysavetime, forecastdays;
extern int nowyear, nowmonth, nowday, nowhour, nowminute, nowsecond, nowwday;

// error
extern RTC_DATA_ATTR int errorcode;
extern RTC_DATA_ATTR int errorcount;

// forecast variables
extern String forecastDay[3];
extern int forecastmaxTmpC[3];
extern int forecastminTmpC[3];
extern int forecastmaxRH[3];
extern int forecastminRH[3];
extern String forecastPSR[3];
extern int forecasticon[3];

// Current weather variables
extern int rainmax;
extern int rainmin;
extern int weathericon, uv;
extern int CurrentTmpC;
extern int CurrentRH;
extern String todayrainchance;
extern int todaymintmpC, todaymaxtmpC;
extern int todayminRH, todaymaxRH;
extern int AQI;
extern String Vis;
extern String sunrise;
extern String sunset;
extern String warnicon[5];

// configurations
extern String ssid, pass, rainlocation, tmpClocation, AQIstation;
extern int updateinterval, wakehour, sleephour;
extern String ipString;
extern bool LED_on;
extern int LED_pin;

// others
extern String currentVersion;
extern int display_rotation;
extern int maxretrytimes;
extern int nextupdatetime;

// other ungrouped functions
void LEDWrite(int pinnumber, bool LEDstate);

// xTask LED blink functions
extern TaskHandle_t longflashhandle;
void LEDlongflashtask(void *parameter);
void LEDlongflashstart();
void LEDlongflashstop();
//

void wifiConnect();

#endif
