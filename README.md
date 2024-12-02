<h>InkWeatherMini</h>

**If you already own an InkWeatherMini, click to go to the [English Manual](Manual_en.md)**

**如你已擁有InkWeatherMini，可以前往[中文使用說明](Manual_zh.md)**


Table of Content



<h1>## Introduction</h1>

This is WeatherMini, a mini e-ink weather station that displays the current weather condition with air quality index and 2 day forcasts. Currently, only 2.9inches eink screens and b/w are supported.


<!--
Colored eink display can be used in thoery (bwr/ bwy/ 4color), but they have not been tested as I do not have any 2.9inches color eink with me. However, please note that they take longer to refresh.

The project uses Hong Kong Observatory Open Data API for weather data, as well as RSS from Hong Kong Enviromental Protection Department for the AQHI, so can be used in Hong Kong only. No sign up is required as of the time of this project. -->

InkWeatherMini does not collect your data in any form, your configuration data are only stored locally in your device. All interactions are limited to the required API calls directly made to HKO, EPD and NTP server to get the weather information and local time.


**Why e-ink?**  E-ink displays can kept its last displayed content without power supply, so the device is in deep sleep mode between readings intervals. It only wakes up for around ~30-60s on each update (to connect to wifi to fetch NTP and weather data and then 20s for refreshing the screen). It can be viewed under bright light. Depending on the battery and update interval you choose, it will last months without needing to be charged.


*****
**If you are new to e-ink, please take extra care when handling the e-ink screen. They are SUPER FRAGILE, if you drop it or put pressure on it, it is going to be a goner :'(**

Flickering on screen refreshes is, unfortunately, normal for e-ink as far as the current technology goes. For panels that are black and white only, the flickering time is much shorter. See videos below for references.
*****


<h1>##How it works<h1>

<p>


The WeatherMini first connects to the internet, syncing time with NTP server, then it requests data from HKO API and fetch data from EPD AQHI RSS. 

If any errors happen during any steps above, the relevant error message will be displayed. It will return again in 5 mins. If error still persists after 5 attempts, all subsequent retries will take place every 120 mins. The screen will not update until error is resolved. Error count is saved on the ESP32's NSV using Preferences library, so the count will remain until you have a successful load or if you clear the NVS manually.

After sucessfully getting data, the display will refresh. Once the update is finished, the ESP32 will go into deep sleep until the next wake up interval. Because the internal timer of the ESP32 has about ~5% (according to google), there will be some fluctuations for the update time. 
The display is set to update at around 5 minutes pass each hour, since it will only display the hour in which the data is updated.

You can always manually press RST on your ESP32 to get data instantly.



##Building your own InkWeatherMini

WIP, please stay tuned!

<!--

WIP

<h2>Materials needed: </h2>
I do not recevie any form of rebate from the manufacturer(s), you are free to try out different ones.


**Verions explained**
Two versions are provided, choose the one base on your need.

**- Source code version: More flexibility *requires basic Arduino knowledge***, you are free to tweak the code to your liking (but please don't redistribute them). You can use other panels that are supported by GxEPD2.
You can also use other ESP32 board.  Configurations are hardcoded in confirgurations.cpp and display_config.h
(Coming soon! The code still needs a proper clean up and refine for readibility)


**- bin version: For end user.**
The .bin supports only b/w display and includes async web server which allows changing configs via browser and web OTA. 
You have to use b/w display with the SSD1680 I/C (e.g. GDEY029T94 from Good-display) and ESP32-C3 SuperMini with a push button and connect exactly in this tutorial.





**E-ink screen:**
- Any 2.9inches eink display with a resolution of 128x296 that are supported by <a href="https://github.com/ZinggJM/GxEPD2">GxEPD2</a> should do.
- This have been tested on B/W display only.
- Note that while this project is written to also support color display, but they have not been tested.


**Microcontroller board:**
The ESP32-C3 is used to keep costs down and to keep things tiny: 
- ESP32-C3 SuperMini: You can get these for like $2USD each, there are all sort of brands out there since it seem to be open-sourced? But you have to add external charging circuit. Do get the one with 4mb on board flash.
  
**The ESP32-C3 uses slightly different deep sleep wake up functions than the other ESP32 variants, so if you decide to use other boards and intended to use webserver, you need to define the pins and the code accordingly. See software settings section.
If you want it to be battery powered, either pick a board with on-board charging circuit or add your own. Always check manufacturers' datasheets carefully!
The code supports ADC battery voltage reading (if #define batterypin /*pin_number*/), but you would probably want to adjust the voltage/charge% values since batteries are all different.



**E-ink adaptor board:**
- e.g. DESPI-C02
You need an adaptor board to connect the eink screen to your ESP32. There are other brands out there, some options are cheaper and they come in different forms.
You can choose any you like, the DESPI-C02 is just an example.

**Push button x 1:**
Choose any you like. Compulsory if you want to load the .bin file directly, optional if you choose to hardcode configurations directly in the code.

**Resistor x 2, capacitor x 1, Lithium battery, charging circuit: (Optional)**
Only if you want to use lithium battery to run your device and if you know what you are doing. I am not going to go through this in details.
If you don't know what you are going, just power the device using 5V input to ESP32C3's USB port.
I personally use 2x 1MOhm resistor with 1nF capacitor, not super accurate but gives an idea of battery charge.
TP4056 is probably the easiest to work with.
Battery icon will be shown in bottom left if valid voltage is read.


**Others:**
You will also need your soldering tools/ materials (wires/ protoboard etc).
I also suggest getting FFC cables and connectors (24pin / 0.5mm) so you have more flexibility with your screen and your board since the FPC cable on the display is fairly short.




<h2>Connecting the wires:</h2>

**Do not feed 5V to the display, most of them operates at 3V3**
- DESPI-C02 ->	ESP32-C3 SuperMini: (Or follow the silkscreen on your adaptor board)

- BUSY	->	5

- RES	-> 0

- D/C	->	10

- CS	->	7

- SCK	->	4

- SDI	->	6

- GND	->	GND 

- 3V3	->	3V3


**Push button**
One leg to buttonpin (3 for the .bin version), the other to 3V3



Then connect the screen to the connector on the board. Pay attention to the orientation of the screen, usually both the display and the adaptor board should be facing up , but this might be the opposite for some adaptor boards. If screen doesn't work, try the other way round.




*******


<h2>Software Installation: </h2>
(You may skip this if you know what you are doing)

Software:
- Install Arduino IDE:
https://www.arduino.cc/en/software

**PLACEHOLDER TO BE ADDED LATER**

<h2>Configuable settings:</h2>

**configurations.cpp**

Settings with info are found in the header file "configurations.h"

- ssid - your wifi SSID

- password - your wifi password

- wakehour - The hour which the first update of the day starts, in 24 hour time. *Please keep it below 10, otherwise today min max temperature cannot show correctly.

*The min and max temperature of the current day is actually taken from the forecast and not from current weather, and the forecast of the current day disapper sometime after 10-11am, so if the weather postcard has not gotten any data from the HKO since the day changed, it will not have any actual data for the min max for the day (instead, the min max shown will be the min max for the next day.) I have checked HKO's API numberous time, but if you find the min max temperature for the current day elsewhere, let me know and I can adjust the code.

- sleephour - The hour of the last update**, in 24 hour time (e.g. 10pm = 22).

- updateinterval - The frequency which the display is updated, in hours**.

**Update interval precedes sleep hour, so say you have set 3 hour update interval with 8am(8) wakehour and 10pm(22) sleephour, the last update will take place at 11pm.

- tmpClocation: Location of where the current temperature is displayed. Please refer to "locations.h" for the list of avaiable locations.

- rainlocation: Location of where the rainfall of last hour is displayed. Please refer to "locations.h" for the list of avaiable locations. 

- batterypin: assign the ADC pin to read battery voltage, simply comment// this line if not needed. You may also want to change the voltage to % level depending on the voltage divider and the type of battery you are using.


**Display Configurations:**

Configuration for the display is in "displayconfig.h"

Choose the correct display and display class for your eink panel, and define the pins you use.

-->


********

If you notice anything wrong with the code or have any suggestions, just let me know, thank you.

********

All icons are drawn by mahbird. (a.k.a. the project creator)

<h1>##Credits: </h1>

These are the libaries and resources used in this project:

ArduinoJson by bblanchon: https://github.com/bblanchon/ArduinoJson

GxEPD2 by ZinggJM: https://github.com/ZinggJM/GxEPD2

u8g2 fonts by olikraus: https://github.com/olikraus/u8g2

Adafruit_GFX by Adafruit: https://github.com/adafruit/Adafruit-GFX-Library

ESPAsyncWebServer/ AsyncTCP by me-no-dev: https://github.com/me-no-dev/ESPAsyncWebServer / https://github.com/me-no-dev/AsyncTCP

Fonts used:
Spleen by fcambus: https://github.com/fcambus/spleen
open-huninn-font: https://github.com/justfont/open-huninn-font

Font converter: https://rop.nl/truetype2gfx/


Data service provider:

Weather data: https://www.hko.gov.hk/en/weatherAPI/doc/files/HKO_Open_Data_API_Documentation.pdf (according to documentation version 1.2)

AQI: https://www.aqhi.gov.hk/psi/dd/hk_aqhiforecast_tc.pdf

NTP: time.nist.gov, pool.ntp.org, stdtime.gov.hk, time.google.com

##[License](License)

