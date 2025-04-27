#ifndef drawfunctions_h
#define drawfunctions_h

#include "configurations.h"
#include "display_config.h"
#include "icons/icons_others.h"
#include "icons/icons_warnings.h"
#include "icons/icons_weather.h"
#include "fonts/tcfont.h"

#include <U8g2_for_Adafruit_GFX.h>
extern U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

enum alignX {LEFT, RIGHT, CENTER};
enum alignY {TOP, MID, BOTTOM};


//common
void drawString(int x, int y, String text, alignX alignx, alignY aligny);
void drawStringdisplay(int x, int y, String text, alignX alignx, alignY aligny); // The converted font has wrong parameters so this does not work as expected, might use this for another project but not this one 

//Weather render
void drawSRicon(int x, int y, int r);
void drawSSicon(int x, int y, int r);
void drawAQicon(int x, int y, int aq, alignY aligny);
void drawUVbar(int x, int y , int w, int h, int UVI, alignY aligny);



/**
 * @draw warnicon, 32x32px each icon with 2 px spacing between each icon
 *
 * @param x x start
 * @param y y start
 * @param max_number max number of
 * @param alignx LEFT / RIGHT only (nothing will be drawn with CENTER)
 */
void drawwarnicon(int x, int y, int max_number, alignX alignx); 
void drawWeathericon(int x, int y, int weathericon);




/**
 * @draw battery icon
 *
 * at 1x size is 17x7 (also another version for 21x8 in drawfunctions.cpp)
 *
 * @param x x start
 * @param y y start
 * @param ws witdh scale, 1 for 1x and 2 for 2x etc
 * @param hs height scale, 1 for 1x and 2 for 2x etc
 * @param l battery level, min 0 max 4
 */

void drawbatteryicon (int x, int y, int ws, int yx, int l);





//Update screen related functions
void drawscreen(String text);

void drawprefscreen();
void drawfailscreen();
void drawOTAscreen();
void drawNoUpdatescreen();
void drawSelectscreen();
void drawsoftAPscreen();

void drawerrorscreen();
void drawinitialscreen();

#endif