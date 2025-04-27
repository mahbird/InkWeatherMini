#ifndef display_config_h
#define display_config_h

#include "configurations.h"

// Modifying the pins below according to your wiring
#define CS 7
#define DC 10
#define RST 0
#define BSY 5

#define batterypin A1 // Use two voltage divider of same value with ADC to monitor LiPo battery charge.
// simply comment// this line if not needed. You may also want to change the voltage to % level depending on the voltage divider and the type of battery you are using.

// You may need to change GxEPD2_DRIVER_CLASS) below depending on your eink panel model

#define display_BW
// #define display_4G // Use greyscale on supported monochrome displays, would increase refresh time
// #define display_BWR //3 color - black / white / red displays
// #define display_BWY //3 color - black / white / yellow displays
// #define display_4C // 4 color displays - Actually currently there is no supported model at this time of writing,
//  but just leaving it here, in case it is supported in the future or if you want to add your own panel for it.

#ifdef display_BW
#include <GxEPD2_BW.h>
#define GxEPD2_DISPLAY_CLASS GxEPD2_BW
#define GxEPD2_DRIVER_CLASS GxEPD2_290_GDEY029T94 // or other panels from GxEPD2

#elif defined display_4G
#include <GxEPD2_4G_G.h>
#define GxEPD2_DISPLAY_CLASS GxEPD2_4G_4G
#define GxEPD2_DRIVER_CLASS GxEPD2_290_GDEY029T94 // or other panels from GxEPD2

#elif defined display_BWR
#include <GxEPD2_3C.h>
#define GxEPD2_DISPLAY_CLASS GxEPD2_3C
#define GxEPD2_DRIVER_CLASS GDEH029Z13

#elif defined display_BWY
#include <GxEPD2_3C.h>
#define GxEPD2_DISPLAY_CLASS GxEPD2_3C
#define GxEPD2_DRIVER_CLASS GxEPD2_290_C90c

#else
#include <GxEPD2_4C.h>
#define GxEPD2_DISPLAY_CLASS GxEPD2_4C
#define GxEPD2_DRIVER_CLASS DRIVERPLACEHOLDER // see note in above section

#endif

// Anything below this line should not need changing

#define ENABLE_GxEPD2_GFX 1
#define MAX_DISPLAY_BUFFER_SIZE 65536ul
#define MAX_HEIGHT(EPD) (EPD::HEIGHT <= MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8) ? EPD::HEIGHT : MAX_DISPLAY_BUFFER_SIZE / (EPD::WIDTH / 8))
extern GxEPD2_DISPLAY_CLASS<GxEPD2_DRIVER_CLASS, MAX_HEIGHT(GxEPD2_DRIVER_CLASS)> display;

#define smallfont u8g2_font_spleen16x32_mf
#define tinyfont u8g2_font_spleen6x12_mf
/*!!
If you encounter compile error in this part, most likely you missed the note in the main file:
U8g2_for_Adafruit_GFX is used in this project, but the font list is not as updated as the main u8g2
The spleen font family used is one of them.
You need to download the both the u8g2 and U8g2_for_Adafruit_GFX library, then replace the font list in between /* start font list 
 /* C++ compatible in U8g2_for_Adafruit_GFX/src/U8g2_for_Adafruit_GFX.h with the ones in u8g2/csrc/u8g2.h
Then replace everything but the #include "u8g2_fonts.h" in U8g2_for_Adafruit_GFX/src/u8g2_fonts.c  with u8g2/csrc/u8g2_fonts.c
Once you are done replacing the files, you can uninstall the main u8g2 library
If you find this too complicated, you may consider trying different fonts
!! */


#ifdef display_BW

#define black GxEPD_BLACK
#define white GxEPD_WHITE
#define yellow GxEPD_WHITE
#define red GxEPD_BLACK

#elif defined display_4G

#define black GxEPD_BLACK
#define white GxEPD_WHITE
#define yellow GxEPD_LIGHTGREY
#define red GxEPD_DARKGREY

#elif defined display_BWR

#define black GxEPD_BLACK
#define white GxEPD_WHITE
#define yellow GxEPD_YELLOW
#define red GxEPD_BLACK

#elif defined display_BWY

#define black GxEPD_BLACK
#define white GxEPD_WHITE
#define yellow GxEPD_WHITE
#define red GxEPD_RED

#else // display_4C

#define black GxEPD_BLACK
#define white GxEPD_WHITE
#define yellow GxEPD_YELLOW
#define red GxEPD_RED
#endif

#endif