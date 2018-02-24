/**********
The MIT License (MIT)
Copyright (c) 2015 by bbx10node@gmail.com
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
**********/
//https://www.wunderground.com/weather/il/haifa/32.797,35.005
uint8_t step =0;
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#ifndef min
#define min(x,y) (((x)<(y))?(x):(y))
#endif
#ifndef max
#define max(x,y) (((x)>(y))?(x):(y))
#endif
#include <ArduinoJson.h>
#define RED 5
#define GREEN 4
#define BLUE 0
int PIR_Input = D6;

#define ON LOW
#define OFF HIGH
#define FIREBASE_HOST "smartumbrella-38d6e.firebaseio.com"
#define FIREBASE_AUTH "b6UumNPNHJaKdNhttjN3edav2W7SqzSPbfzgkTtE"

const char SSID[]     = "Elizur kelner";
const char PASSWORD[] = "31213121";

// Use your own API key by signing up for a free developer account.
// http://www.wunderground.com/weather/api/
#define WU_API_KEY "08196a64ffb6e050"

// Specify your favorite location one of these ways.
//#define WU_LOCATION "CA/HOLLYWOOD"


// US ZIP code
//#define WU_LOCATION ""
//#define WU_LOCATION "90210"

// Country and city
#define WU_LOCATION "Australia/Darwin"

#define WUNDERGROUND "api.wunderground.com"

// HTTP request
const char WUNDERGROUND_REQ[] =
    "GET /api/" WU_API_KEY "/conditions/q/" WU_LOCATION ".json HTTP/1.1\r\n"
    "User-Agent: ESP8266/0.1\r\n"
    "Accept: /\r\n"
    "Host: " WUNDERGROUND "\r\n"
    "Connection: close\r\n"
    "\r\n";

void setup()
{
  pinMode(PIR_Input,INPUT);

  pinMode(RED,OUTPUT);
  pinMode(GREEN,OUTPUT);
  pinMode(BLUE,OUTPUT);
  Serial.begin(115200);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(SSID);

  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }

  Serial.println();
  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
 
}

static char respBuf[4096];
static const char *location;
static const char *rain;
static const char *deg;

int n = 0;
void loop()
{
   
   
  digitalWrite(RED,OFF);
  digitalWrite(GREEN,OFF);
  digitalWrite(BLUE,OFF);
  // TODO check for disconnect from AP

  // Open socket to WU server port 80
  Serial.print(F("Connecting to "));
  Serial.println(WUNDERGROUND);

  // Use WiFiClient class to create TCP connections
  WiFiClient httpclient;
  const int httpPort = 80;
  if (!httpclient.connect(WUNDERGROUND, httpPort)) {
    Serial.println(F("connection failed"));
    return;
  }

  // This will send the http request to the server
  Serial.print(WUNDERGROUND_REQ);
  httpclient.print(WUNDERGROUND_REQ);
  httpclient.flush();

  // Collect http response headers and content from Weather Underground
  // HTTP headers are discarded.
  // The content is formatted in JSON and is left in respBuf.
  int respLen = 0;
  bool skip_headers = true;
  while (httpclient.connected() || httpclient.available()) {
    if (skip_headers) {
      String aLine = httpclient.readStringUntil('\n');
      //Serial.println(aLine);
      // Blank line denotes end of headers
      if (aLine.length() <= 1) {
        skip_headers = false;
      }
    }
    else {
      int bytesIn;
      bytesIn = httpclient.read((uint8_t *)&respBuf[respLen], sizeof(respBuf) - respLen);
      Serial.print(F("bytesIn ")); Serial.println(bytesIn);
      if (bytesIn > 0) {
        respLen += bytesIn;
        if (respLen > sizeof(respBuf)) respLen = sizeof(respBuf);
      }
      else if (bytesIn < 0) {
        Serial.print(F("read error "));
        Serial.println(bytesIn);
      }
    }
    delay(1);
  }
  httpclient.stop();

  if (respLen >= sizeof(respBuf)) {
    Serial.print(F("respBuf overflow "));
    Serial.println(respLen);
    return;
  }
  // Terminate the C string
  respBuf[respLen++] = '\0';
  Serial.print(F("respLen "));
  Serial.println(respLen);
  //Serial.println(respBuf);

  if (showWeather(respBuf)) {
    for(int i =0;i<1800;i++){
      if(digitalRead(PIR_Input)){
        char l[20];
        itoa(n,l,10);

        char r[20];
        itoa(n,r,10);

        char d[20];
        itoa(n,d,10);
        n++;

        char tmp = '/';
        append(l, tmp);
        append(r, tmp);
        append(d, tmp);

        tmp = 'l';
        append(l, tmp);

        tmp = 'r';
        append(r, tmp);

        tmp = 'd';
        append(d, tmp);
        
        Firebase.pushString(l, location );
        Firebase.pushString(r, rain );
        Firebase.pushString(d, deg );
    
        // handle error
        if (Firebase.failed()) {
          Serial.print("setting /number failed:");
          Serial.println(Firebase.error());  
          return;
        }
        delay(5*1000);
        break;
      }
    delay(100);
    }
  }
    
}

bool showWeather(char *json)
{
  StaticJsonBuffer<3*1024> jsonBuffer;

  // Skip characters until first '{' found
  // Ignore chunked length, if present
  char *jsonstart = strchr(json, '{');
  //Serial.print(F("jsonstart ")); Serial.println(jsonstart);
  if (jsonstart == NULL) {
    Serial.println(F("JSON data missing"));
    return false;
  }
  json = jsonstart;

  // Parse JSON
  JsonObject& root = jsonBuffer.parseObject(json);
  if (!root.success()) {
    Serial.println(F("jsonBuffer.parseObject() failed"));
    return false;
  }

  // Extract weather info from parsed JSON
  JsonObject& current = root["current_observation"];
  const float temp_f = current["temp_f"];
  Serial.print(temp_f, 1); Serial.print(F(" F, "));
  const float temp_c = current["temp_c"];
  deg = current["temp_c"];
  Serial.print(temp_c, 1); Serial.print(F(" C, "));
  const char *humi = current[F("relative_humidity")];
  Serial.print(humi);   Serial.println(F(" RH"));
  const char *weather = current["weather"];
  Serial.println(weather);
  Serial.print(F("rain "));
  const char *rainIn = current["precip_today_in"];
  const char *rainMet = current["precip_today_metric"];
  rain = current["precip_today_metric"];
  Serial.print(rainIn);  Serial.print(F(" in, "));
  Serial.print(rainMet);  Serial.println(F(" mm"));
  led(rainMet);
  const char *pressure_mb = current["pressure_mb"];
  Serial.println(pressure_mb);
  const char *observation_time = current["observation_time_rfc822"];
  Serial.println(observation_time);
  // Extract local timezone fields
  const char *local_tz_short = current["local_tz_short"];
  Serial.println(local_tz_short);
  const char *local_tz_long = current["local_tz_long"];
  Serial.println(local_tz_long);
  location = current["local_tz_long"];
  const char *local_tz_offset = current["local_tz_offset"];
  Serial.println(local_tz_offset);

  return true;
}
int led(const char *c){
  int x = atof(c);
   if(x > 0){
    if(x < 3){
      digitalWrite(GREEN,ON);//GREEN
    }else{
      if(x<8){
         digitalWrite(BLUE,ON);//BLUE
        }else{
         digitalWrite(RED,ON);//RED
      }
    }
   }
}
void append(char* s, char c) {
        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
}

