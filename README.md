**WIP**


<h1>WeatherMini</h1>

This is WeatherMini, a mini e-ink weather station that displays the current weather condition with air quality index and 2 day forcasts. Currently, only 2.9inches eink screens and b/w are supported.

Colored eink display can be used in thoery (bwr/ bwy/ 4color), but they have not been tested as I do not have any 2.9inches color eink with me. However, please note that they take longer to refresh.

The project uses Hong Kong Observatory Open Data API for weather data, as well as RSS from Hong Kong Enviromental Protection Department for the AQHI, so can be used in Hong Kong only. No sign up is required as of the time of this project.



**Why e-ink?**  E-ink displays can kept its last displayed content without power supply, so the device is in deep sleep mode between readings intervals. It only wakes up for around ~30-60s on each update (to connect to wifi to fetch NTP and weather data and then 20s for refreshing the screen). It can be viewed under bright light. Depending on the battery and update interval you choose, it will last months without needing to be charged.


*****
**If you are new to e-ink, please take extra care when handling the e-ink screen. They are SUPER FRAGILE, if you drop it or put pressure on it, it is going to be a goner :'(**

Flickering on screen refreshes is, unfortunately, normal for e-ink as far as the current technology goes. For panels that are black and white only, the flickering time is much shorter. See videos below for references.
*****




WeatherMini Explained</h2>
<p>


The WeatherMini first connects to the internet, syncing time with NTP server, then it requests data from HKO API and fetch data from EPD AQHI RSS. 

If any errors happen during any steps above, the relevant error message will be displayed. It will return again in 5 mins. If error still persists after 5 attempts, all subsequent retries will take place every 120 mins. The screen will not update until error is resolved. Error count is saved on the ESP32's NSV using Preferences library, so the count will remain until you have a successful load or if you clear the NVS manually.

After sucessfully getting data, the display will refresh. Once the update is finished, the ESP32 will go into deep sleep until the next wake up interval. Because the internal timer of the ESP32 has about ~5% (according to google), there will be some fluctuations for the update time. 
The display is set to update at around 5 minutes pass each hour, since it will only display the hour in which the data is updated.

You can always manually press RST on your ESP32 to get data instantly.


<h2>Materials needed: </h2>
(Prices are the displayed price as of time of this post (1 Sep, 2024),  for reference only , excludes shipping / discounts). I do not recevie any form of rebate from the manufacturer(s), you are free to try out different ones.
<p>
<img src="https://github.com/user-attachments/assets/61679cf7-91b8-4f46-968a-c884e56e1c70" height=400px></p>


**E-ink screen:**
- Any 2.9inches eink display with a resolution of 128x296 that are supported by <a href="https://github.com/ZinggJM/GxEPD2">GxEPD2</a> should do.
Note that while this project is written to also support 3 and 4 color display, they have not been tested.

**Microcontroller board:**
The ESP32-C3 is used to keep costs down and to keep things tiny: 
- ESP32-C3 SuperMini: You can get these for like $2USD each, there are all sort of brands out there since it seem to be open-sourced? But you have to add external charging circuit. Do get the one with 4mb on board flash.
  
**The ESP32-C3 uses slightly different deep sleep functions than the other ESP32 variants, so if you decide to use other boards, you need to adjust the pins and the code accordingly.


**E-ink adaptor board:**
- e.g. DESPI-C02: - USD $6.99 on their official website/ CNY 30 on taobao
You need an adaptor board to connect the eink screen to your ESP32. There are other brands out there, some options are cheaper and they come in different forms, above is just a suggestion and is the one used in this tutorial. 



**Optional Parts**
This is more complicated and 

**Battery Charging Circuit and Lithium Battery (Optional)**

**470k ohm resistors x 2 (Optional)**

**0.1uF capacitor x 1 (Optional)**

**Push Button x 2 (Optional)** 
Any you want to use.

**Switch x 1 (Optional)**
Any you want to use.

**Others:**
You will also need your soldering tools/ materials (wires/ protoboard etc).
I also suggest getting FFC cables and connectors (24pin / 0.5mm) so you have more flexibility with your screen and your board since the FPC cable on the display is fairly short.




<h2>Connecting the wires:</h2>

- DESPI-C02 ->	ESP32-C3 SuperMini:

- BUSY	->	14

- RES	->	9

- D/C	->	1

- CS	->	18

- SCK	->	23

- SDI	->	22

- GND	->	GND 

- 3V3	->	3V3


**Do not feed 5V to the display, most of them operates at 3V3**

Then connect the screen to the connector on the board. Pay attention to the orientation of the screen, usually both the display and the adaptor board should be facing up , but this might be the opposite for some adaptor boards. If screen doesn't work, try the other way round.

**Optional Parts - Battery with charging, **

Charging circuit and lithium battery. In this example I use TP4056 module for simplicity. The BAT OUT+ goes to 5V pin of the ESP32-C3, connect a 



*******


<h2>Software Installation: </h2>
(You may skip this if you know what you are doing)

Software:
- Install Arduino IDE:
https://www.arduino.cc/en/software

Arduino IDE 2.3.2 is the last version at the time of this project which is the version being used. Choose the appropiate option depending on your OS.


- Install the Firebeetle 2 ESP32-C6 board in arduino: (Or relevent boards if you are using others)
https://wiki.dfrobot.com/SKU_DFR1075_FireBeetle_2_Board_ESP32_C6#target_5

- Install the following libraries:

ArduinoJson by bblanchon: https://github.com/bblanchon/ArduinoJson

GxEPD2 by ZinggJM: https://github.com/ZinggJM/GxEPD2

u8g2 fonts by olikraus: https://github.com/olikraus/u8g2

Adafruit_GFX by Adafruit: https://github.com/adafruit/Adafruit-GFX-Library







- Download the code on this project's main page, (click on <> code -> Download ZIP). 
![download](https://github.com/user-attachments/assets/1ea66c3d-8c26-47a8-86db-87f056e0b4fb)

Extract the zip files and open Weather_Postcard.ino in Arduino IDE, change the configuration settings as described in the section below, select your board and upload. It will take a while for the code to compile for the first time so please be patient.


<h2>Configuable settings:</h2>

**configurations.h**

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

- customtext: Any text you would like to display on the last line, though it can't show too many!

- time24h: Display the last update time in 24h or a.m./p.m.

- batterypin / nobattery: whether you are using ADCbatteryread, no need to change if you are using Firebeetle ESP32-C6


**Display Configurations:**

Configuration for the display is in "displayconfig.h"

The default setting is for Firebeetle ESP32-C6 with the 4 color E-Ink display, no need to change anything if you are are using the said combination.

If you are using b/w display with the Firebeetle ESP32-C6, add "//" before #define display_4C and delete the "//" before #define display_BW (There is also display_3C for B/W/R display if you have old panels that you want to use, but it will be missing the yellow colors)

If you are using other microcontroller boards, you need to define your own pins. 


********

This is my first project on github, if you notice anything wrong with the code, let me know, thank you.

********


本天氣明信片以及圖示均為mahbird基於香港天文台的免費資訊制作及繪畫，於Arduino IDE 2.3.2編寫，並於github免費發佈，僅供個人使用，
轉載需保留原訊息，不得用於其他地方，可按需要自行修改但不得作二次發佈，不得用於商業用途。
本程序基於香港天文台的免費API (https://www.hko.gov.hk/tc/weatherAPI/doc/files/HKO_Open_Data_API_Documentation_tc.pdf) (Version 1.11, Date : Nov, 2023)
如未來API有變更，可能令本程序不相容

The Weather postcard and all icons are created and drawn by mahbird on Github, written in Arduino IDE 2.3.2, based on information available from the Hong Kong Observatory. 

The code is distributed for free on github. For personal use only. Please retain all original messages when posting on other sites. You are free to modify this code as you like, but do not redistribute modified code and do not use them elsewhere. Commerical use is not allowed.

Please note that the weather informations are taken from the publicly available Hong Kong Observatory Open Data API (https://www.hko.gov.hk/en/weatherAPI/doc/files/HKO_Open_Data_API_Documentation.pdf) (Version 1.11, Date : Nov, 2023). It it possible that this software may not be compatiable if the API changed in the future.




Special thanks to the developer for the libraries (and the associated libararies in the libraries) used in this project:

ArduinoJson by bblanchon: https://github.com/bblanchon/ArduinoJson

GxEPD2 by ZinggJM: https://github.com/ZinggJM/GxEPD2

u8g2 fonts by olikraus: https://github.com/olikraus/u8g2

Adafruit_GFX by Adafruit: https://github.com/adafruit/Adafruit-GFX-Library


The project is designed to use with ESP32-C3 SuperMini, due to its small form factor, low price and availablity.

Eink
