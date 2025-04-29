#include "configurations.h"





int nowyear, nowmonth, nowday, nowhour, nowminute, nowsecond, nowwday;
String todayrainchance = "";
int todaymintmpC = 0;
int todaymaxtmpC = 0;
int todayminRH = 0;
int todaymaxRH = 0;

int AQI = 0;

//back up data if the current station fails
static String tmpClocation0 = "Hong Kong Observatory";

static int nowdaycheck = 0;
static bool updateSRS = false;



const char baseURL[] = "https://data.weather.gov.hk/weatherAPI/opendata/weather.php?dataType="; // HKO weather
const String cwrq = "rhrread&lang=en"; // CurrentWeather request
const String fcrq= "fnd&lang=en"; // Forcast request
const String baseURL2 = "https://data.weather.gov.hk/weatherAPI/opendata/opendata.php?dataType="; // HKO OpenWeather
const String vbrq = "LTMV&lang=en&rformat=json"; // visibility requestH
const String warnrq ="warnsum&lang=en"; // current weather warning
//char srsrq [60]; // moved to getSRS();




//String forecastdate[3]; //forecast variables
String forecastDay[3];
int forecastmaxTmpC[3];
int forecastminTmpC[3];
int forecastmaxRH[3];
int forecastminRH[3];
String forecastPSR[3];
int forecasticon[3];


 // Current weather variables
int rainmax;
int rainmin;

int weathericon, uv;


int CurrentTmpC = -1;
int CurrentRH = -1;

//other variables
String Vis;
String sunrise;
String sunset;
String warnicon[5];



void getForecast(){
HTTPClient http;
http.begin (baseURL+fcrq);
int httpCode = http.GET();

if (httpCode > 0) {
  String result = http.getString();

JsonDocument filter;

JsonObject filter_weatherForecast_0 = filter["weatherForecast"].add<JsonObject>();
filter_weatherForecast_0["forecastDate"] = true;
filter_weatherForecast_0["week"] = true;
filter_weatherForecast_0["forecastMaxtemp"]["value"] = true;
filter_weatherForecast_0["forecastMintemp"]["value"] = true;
filter_weatherForecast_0["forecastMaxrh"]["value"] = true;
filter_weatherForecast_0["forecastMinrh"]["value"] = true;
filter_weatherForecast_0["ForecastIcon"] = true;
filter_weatherForecast_0["PSR"] = true;

JsonDocument doc;

DeserializationError error = deserializeJson(doc, result, DeserializationOption::Filter(filter));

if (error) {
//  //Serial.print("deSerialzeJson(forecast) failed: ");
//  //Serial.println(error.c_str());
  errorcode = 3;
  
   return;
}

int i = 0;
for (JsonObject weatherForecast_item : doc["weatherForecast"].as<JsonArray>()) {

  forecastDay[i] = weatherForecast_item["week"].as<String>(); // "Monday", "Tuesday", ...
  forecastmaxTmpC[i] = weatherForecast_item["forecastMaxtemp"]["value"];
  forecastminTmpC[i] = weatherForecast_item["forecastMintemp"]["value"];
  forecastmaxRH[i] = weatherForecast_item["forecastMaxrh"]["value"]; // 90, ...
  forecastminRH[i] = weatherForecast_item["forecastMinrh"]["value"]; // 60, ...
forecastPSR[i] = weatherForecast_item["PSR"].as<String>(); // "Low", "Medium Low", "Low", ...

  int tmpicon = weatherForecast_item["ForecastIcon"];
   if (tmpicon > 89){tmpicon -= 65;} // Hot 90/ 	Warm 91/ 	Cool 92/	Cold 93 / -> 25-28
   else if (tmpicon > 79){tmpicon -= 61;} // Windy 80/	Dry	81/ Humid 82/	Fog	83/ Mist	84/Haze 85/ -> 19-24
else if (tmpicon > 69){tmpicon -= 59;} // night icon 70 - 77 -> 11-18
    else if (tmpicon > 59){tmpicon -= 55;} // Cloudy	Overcast 60/	Light Rain 61/	Rain 62/	Heavy Rain	63/ Thunderstorms 64/ -> 5-9
   else {tmpicon -= 50;} // Sunny 50/	Sunny Periods 51/	Sunny Intervals	52/Sunny Periods with A Few Showers 53/	Sunny Intervals with Showers	54 -> 0-4
  
   forecasticon[i] = tmpicon;

 

//read today min max from memory, overwrite if date has changed
pref.begin("weather", false);
nowdaycheck = pref.getInt("nowdaycheck", 0);


if (nowday != nowdaycheck) {
pref.putInt("nowdaycheck", nowday);
pref.putInt("todaymintmpC", forecastminTmpC[i]);
pref.putInt("todaymaxtmpC", forecastmaxTmpC[i]);
pref.putInt("todayminRH", forecastminRH[i]);
pref.putInt("todaymaxRH", forecastmaxRH[i]);
pref.putString("todayrainchance", forecastPSR[i]);
updateSRS = true; // get changed SRS

}

todaymintmpC = pref.getInt("todaymintmpC", 0);
todaymaxtmpC = pref.getInt("todaymaxtmpC", 0);
todayminRH = pref.getInt("todayminRH", 0);
todaymaxRH = pref.getInt("todaymaxRH", 0);
todayrainchance = pref.getString("todayrainchance");
sunrise = pref.getString("sunrise", "");
sunset = pref.getString("sunset", "");
pref.end();


int datecheckint = nowyear*10000 + nowmonth*100 + nowday;
if (weatherForecast_item["forecastDate"].as<String>() ==  String(datecheckint)){
   i-=1; 
//Serial.println("Skipped forecast for today");
} 

i++;
if (i>forecastdays-1){break;}

}

//errorcode = 0;
http.end();}
else {//Serial.println("Failed to get forecast information");
errorcode = 3;}
}



void getCurrentWeather(){ // Get current weather data
http.begin (baseURL+cwrq);
int httpCode = http.GET();

if (httpCode > 0) {
  String result = http.getString();


JsonDocument filter;

JsonObject filter_rainfall_data_0 = filter["rainfall"]["data"].add<JsonObject>();
filter_rainfall_data_0["place"] = true;
filter_rainfall_data_0["max"] = true;
filter_rainfall_data_0["min"] = true;
filter["icon"] = true;
filter["uvindex"]["data"][0]["value"] = true;

JsonObject filter_temperature_data_0 = filter["temperature"]["data"].add<JsonObject>();
filter_temperature_data_0["place"] = true;
filter_temperature_data_0["value"] = true;
//filter["tcmessage"] = true;

JsonObject filter_humidity = filter["humidity"].to<JsonObject>();
//filter_humidity["recordTime"] = true;
filter_humidity["data"][0]["value"] = true;

JsonDocument doc;

DeserializationError error = deserializeJson(doc, result, DeserializationOption::Filter(filter));

if (error) {
 // //Serial.print("deSerialzeJson(currentweather) failed: ");
 // //Serial.println(error.c_str());
    errorcode = 3;
    
 
  return;
}


for (JsonObject rainfall_data_item : doc["rainfall"]["data"].as<JsonArray>()) {


  String rainfall_data_item_place = rainfall_data_item["place"].as<String>(); // "Central & Western District", ...
  int rainfall_data_item_max = rainfall_data_item["max"]; // 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ...
  int rainfall_data_item_min = rainfall_data_item["min"];
if (rainfall_data_item["place"] == rainlocation)
{rainmax= rainfall_data_item["max"];
 rainmin= rainfall_data_item["min"];
 }


}

weathericon = doc["icon"][0]; // 50
   if (weathericon > 89){weathericon -= 65;} // Hot 90/ 	Warm 91/ 	Cool 92/	Cold 93 / -> 25-28
   else if (weathericon > 79){weathericon -= 61;} // Windy 80/	Dry	81/ Humid 82/	Fog	83/ Mist	84/Haze 85/ -> 19-24
else if (weathericon > 69){weathericon -= 59;} // night icon 70 - 77 -> 11-18
    else if (weathericon > 59){weathericon -= 55;} // Cloudy	Overcast 60/	Light Rain 61/	Rain 62/	Heavy Rain	63/ Thunderstorms 64/ -> 5-10
   else {weathericon -= 50;} // Sunny 50/	Sunny Periods 51/	Sunny Intervals	52/Sunny Periods with A Few Showers 53/	Sunny Intervals with Showers	54 -> 0-4


uv = doc["uvindex"]["data"][0]["value"]; // 8

for (JsonObject temperature_data_item : doc["temperature"]["data"].as<JsonArray>()) {

String CurrenttmpClocation = temperature_data_item["place"].as<String>(); // "King's Park", "Hong Kong ...
 // CurrentTmpC = temperature_data_item["value"]; // 33, 33, 34, 36, 35, 36, 35, 34, 35, ...
//settmpClocation
if (CurrenttmpClocation==tmpClocation0 && CurrentTmpC == -1) {CurrentTmpC = temperature_data_item["value"];
//Serial.println(tmpClocation0);
}
if (CurrenttmpClocation==tmpClocation)
{CurrentTmpC = temperature_data_item["value"];
//Serial.println(tmpClocation);
}

}

// const char* tcmessage = doc["tcmessage"]; // nullptr

//const char* humidity_recordTime = doc["humidity"]["recordTime"]; // "2024-08-04T15:00:00+08:00"

CurrentRH = doc["humidity"]["data"][0]["value"]; // 61


//errorcode = 0;
http.end();}
else {errorcode = 3;
  //Serial.println("Failed to get Current weather information");
}}


void getVisibility(){ // Get current weather data visibility
http.begin (baseURL2 + vbrq);
int httpCode = http.GET();

if (httpCode > 0) {
  String result = http.getString();


JsonDocument filter;
filter["data"][0] = true;

JsonDocument doc;

DeserializationError error = deserializeJson(doc, result, DeserializationOption::Filter(filter));

if (error) {
  //Serial.print("deSerialzeJson(visibility) failed: ");
  //Serial.println(error.c_str());
    errorcode = 3;
    
  return;
}


//Visibility taken from Central if you wish to use other locations, change the data_0 to data_n (n=1 for Chek Lap Kok /  2 for Sai Wan Ho/ 3 for Waglan Island)
JsonArray data = doc["data"];

JsonArray data_0 = data[0];
//const char* data_0_0 = data_0[0]; // "202408041730"
//const char* data_0_1 = data_0[1]; // "Central"
Vis = data_0[2].as<String>();; // "29 km"
Vis.replace(" ", ""); //removes the space

/*

JsonArray data_1 = data[1];
const char* data_1_0 = data_1[0]; // "202408041730"
const char* data_1_1 = data_1[1]; // "Chek Lap Kok"
const char* data_1_2 = data_1[2]; // "50 km"

JsonArray data_2 = data[2];
const char* data_2_0 = data_2[0]; // "202408041730"
const char* data_2_1 = data_2[1]; // "Sai Wan Ho"
const char* data_2_2 = data_2[2]; // "50 km"

JsonArray data_3 = data[3];
const char* data_3_0 = data_3[0]; // "202408041730"
const char* data_3_1 = data_3[1]; // "Waglan Island"
const char* data_3_2 = data_3[2]; // "45 km"
*/

//errorcode = 0;
http.end();}
else {errorcode = 3;
  //Serial.println("Failed to get visibility information");
}}


void getSRS() { // Get Sunset and Sunrise
if (updateSRS){
char srsrq [60];
snprintf (srsrq, sizeof(srsrq), "SRS&lang=en&rformat=json&year=%d&month=%d&day=%d", nowyear, nowmonth, nowday);
http.begin (baseURL2 + srsrq);
int httpCode = http.GET();

if (httpCode > 0) {
  String result = http.getString();
JsonDocument filter;
filter["data"] = true;

JsonDocument doc;

DeserializationError error = deserializeJson(doc, result, DeserializationOption::Filter(filter));

if (error) {
  //Serial.print("deSerialzeJson(SRS) failed: ");
  //Serial.println(error.c_str());
    errorcode = 3;
    
  return;
}

JsonArray data_0 = doc["data"][0];
//const char* data_0_0 = data_0[0]; // "2024-08-04"
sunrise = data_0[1].as<String>(); // "05:57"
//const char* data_0_2 = data_0[2]; // "12:29"
sunset = data_0[3].as<String>(); // "19:02"


pref.begin("weather", false);
pref.putString("sunrise", sunrise);
pref.putString("sunset", sunset);
pref.end();
//Serial.printf("Sunrise for the day saved: %s, sunset: %s\n", sunrise, sunset);

http.end();
}
else {errorcode = 3;
//Serial.println("Fail to get SRS");
}}




} 



void getwarning(){
http.begin (baseURL + warnrq);
int httpCode = http.GET();

if (httpCode > 0) {
  String result = http.getString();


JsonDocument doc;
DeserializationError error = deserializeJson(doc, result);

if (error) {
//  //Serial.print("deSerialzeJson(warning) failed: ");
 // //Serial.println(error.c_str());
    errorcode = 3;
    
  return;
}


int i=0;
for (JsonPair item : doc.as<JsonObject>()) {
  //const char* item_key = item.key().c_str(); // "WHOT", "WCOLD", "WFNTSA", "WMSGNL"
  //const char* value_name = item.value()["name"]; // "Very Hot Weather Warning", "Cold Weather Warning", ...
  //const char* value_code = item.value()["code"]; // "WHOT", "WCOLD", "WCOLD", "WCOLD"
 // const char* value_actionCode = item.value()["actionCode"]; // "ISSUE", "ISSUE", "ISSUE", "ISSUE"
 // const char* value_issueTime = item.value()["issueTime"]; // "2020-09-24T07:00:00+08:00", ...
 // const char* value_updateTime = item.value()["updateTime"]; // "2020-09-24T07:00:00+08:00", ...


String warncode = item.value()["code"].as<String>(); // "WHOT", "WCOLD", "WCOLD", "WCOLD"
String actioncode = item.value()["actionCode"].as<String>(); // "ISSUE", "ISSUE", "ISSUE", "ISSUE"

if (warncode !="" && actioncode !="CANCEL")
{if (warncode=="WTMW"||warncode=="WFNTSA" ||warncode=="WMSGNL" || warncode=="WTCPRE8" || warncode=="WFROST")
{warnicon[i]="";}
else {warnicon[i]=warncode;
i++;}
}
}


http.end();}
else {//Serial.println("Failed to get warning information");
errorcode = 3;}
}



/*

void getAQI(){ 
http.begin ("https://www.aqhi.gov.hk/epd/ddata/html/out/aqhi_ind_rss_Eng.xml");
int httpCode = http.GET();

if (httpCode > 0) {
String result = http.getString();

int removen = result.indexOf(AQIstation);
result.remove (0, removen+AQIstation.length()+3);
//Serial.println (result);
removen = result.indexOf(" : ");
result.remove(removen, result.length()-removen);
AQI = result.toInt();
//Serial.print("AQ:");
//Serial.println(AQI);
}

http.end();
}

*/


//new AQI

void getAQI() {
  HTTPClient http;
  http.begin("https://www.aqhi.gov.hk/epd/ddata/html/out/aqhi_ind_rss_Eng.xml");
  int httpCode = http.GET();

  if (httpCode > 0) { 
      WiFiClient *stream = http.getStreamPtr();
      // Skip the first x bytes
      int bytesToSkip = 562;
      while (bytesToSkip-- && stream->connected()) {
          stream->read(); 
      }
      String buffer = ""; 
      while (stream->connected() && stream->available()) {
          char c = stream->read(); 
          buffer += c;

          if (buffer.indexOf(AQIstation) != -1 && (buffer.indexOf(AQIstation) < buffer.length() - AQIstation.length() - 10)) {
            int firstColon = buffer.indexOf(" : ");
            int secondColon = buffer.indexOf(" : ", firstColon + 3);
        
            if (firstColon != -1 && secondColon != -1) {
                String aqiString = buffer.substring(firstColon + 3, secondColon);
                AQI = aqiString.toInt();
                //Serial.println("AQI: " + String(AQI));
                return; 
            }
            
        
          }

          else if (buffer.length() > 64) { 
              buffer.remove(0, buffer.length() - AQIstation.length()-11);
          }
      }
  } else {
      Serial.println("HTTP GET failed (getAQI), error: " + String(httpCode));
      errorcode = 3;
  }
  http.end();
}



//connect to wifi and sync with NTP, wifi remains connected 
void wifiNTP(){
  errorcode = 0; // indicates it successfully get to this part
//Serial.println("Contacting NTP");
const char* ntpServers[] = { "time.nist.gov", "pool.ntp.org", "stdtime.gov.hk", "time.google.com" };
    for (const char* server : ntpServers) {
        configTime(3600 * timezone, daysavetime * 3600, server);
        struct tm tmstruct;
        if (getLocalTime(&tmstruct, 5000)) {
nowyear = (tmstruct.tm_year) +1900;
nowmonth =(tmstruct.tm_mon)+1;
nowday = tmstruct.tm_mday;
nowhour = tmstruct.tm_hour;
nowminute = tmstruct.tm_min;
nowsecond = tmstruct.tm_sec;
nowwday = tmstruct.tm_wday;
//snprintf (srsrq, sizeof(srsrq), "SRS&lang=en&rformat=json&year=%d&month=%d&day=%d", nowyear, nowmonth, nowday); //moved to getSRS()
//if (nowday.length()==1){nowday = "0"+nowday;} // something I did for displaying but probably not needed keeping just in case String(nowday)
//if (nowmonth.length()==1){nowmonth = "0"+nowmonth;}
errorcode = 0;
 //Serial.printf("Now is %02d:%02d - %d/%d/%d\n", nowhour, nowminute, nowday, nowmonth, nowyear);
            return;
                   }
                   //Serial.printf("Failed to get time from %s, trying next server...\n", server);
        delay(500); 
    }
    //Serial.println("Fail to get time");
  errorcode = 2;


}
