#include "configurations.h"

// see locations.h

String ssid = "";                     // Your WiFi SSID
String pass = "";                     // Your WiFi password
String rainlocation = "Sham Shui Po"; // Rainfall location, see "locations.h" for list
String tmpClocation = "Sham Shui Po"; // Temperature location, see "locations.h" for list
String AQIstation = "Sham Shui Po";   // Air quality station index location, see "locations.h" for list
int updateinterval = 1;               // default update interval = 1 hour
int wakehour = 8;                     //  the hour which the update starts, set both to same value to disable sleep
int sleephour = 22;                   // the hour of the last update
// if previous update time + interval > sleep hour, it will still update one more time at interval before sleeping.
bool LED_on = true; // Set to false to disable LED blinking
int LED_pin = 8;    // set LED pin number of you are using LED_on

int display_rotation = 3; // Use 1 to rotate the screen 180 degrees

// The following should not need changing since this project is specific to Hong Kong and no point using it elsewhere
int timezone = 8;
int daysavetime = 0;

int forecastdays = 3;
int maxretrytimes = 6;

// Nothing under this line needs to be changed
RTC_DATA_ATTR int errorcode = 0; // No error: 0 / No Wifi: 1 / NTP fail: 2 / JSON fail : 3 / Low battery : 4
RTC_DATA_ATTR int errorcount = 0;

// Common function/ declaration
Preferences pref;


void wifiConnect()
{
    WiFi.begin(ssid, pass);
    unsigned long startcount = millis();
    // Serial.print ("Connecting to Wifi");
    bool buttonstate = 0;
    while (WiFi.status() != WL_CONNECTED)
    { // Serial.print (".");
        LEDWrite(LED_pin, buttonstate);
        buttonstate = !buttonstate;
        delay(500);
        if (millis() - startcount > 15000)
        {
            // Serial.print("\nError 1 - Fail to connect to Wifi\n");
            errorcode = 1;
            // Serial.println("\nFail to connect to Wifi");
            delay(100);
            return;
        }
    }
    // Serial.println("\nWifi Connected");
    // LEDWrite(LED_pin, LOW);
    errorcode = 0;
}

// Ungrouped functions
// LED blinking sets

void LEDWrite(int pinnumber, bool LEDstate)
{
    if (LED_on)
    {
        digitalWrite(pinnumber, LEDstate);
    }
}

TaskHandle_t longflashhandle = NULL;

void LEDlongflashtask(void *parameter)
{
    while (true)
    {
        digitalWrite(LED_pin, HIGH);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        digitalWrite(LED_pin, LOW);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void LEDlongflashstart()
{
    if (LED_on && longflashhandle == NULL)
    {
        xTaskCreate(LEDlongflashtask, "Flash LED", 384, NULL, 1, &longflashhandle);
    }
}

void LEDlongflashstop()
{
    if (longflashhandle != NULL)
    {
        digitalWrite(LED_pin, LOW);
        vTaskDelete(longflashhandle);
        longflashhandle = NULL;
    }
}
