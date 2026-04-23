#include "arduino_secrets.h"
#include <R4HttpClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <PulseSensorPlayground.h>
#include <U8g2lib.h>
#include <Arduino.h>

WiFiSSLClient client;
R4HttpClient http;

const char* _SSID = SECRET_SSID;
const char* _PASS = SECRET_PASS;
const char* _GETSONGAPI = SECRET_GETSONG_API;
const char* _YOUTUBEAPI = SECRET_YOUTUBE_API;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); //Setup u8g2

String song_title = "";
String artist_name = "";
String videoId = "";

const int pulse_PIN = A0; 
int bpm = 100;
int previousPulse = 0;
int threshold = 550;    // Determine which Signal to "count as a beat" and which to ignore

PulseSensorPlayground pulseSensor;

void setup()
{
  Serial.begin(9600);

  pulseSensor.analogInput(pulse_PIN);
  pulseSensor.setThreshold(threshold);
  //pulseSensor.blinkOnPulse(buzzer_PIN);
  pulseSensor.begin();

  u8g2.begin();


  while (!Serial);

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION)
    Serial.println(F("Please upgrade the firmware"));

  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println(F("Communication with WiFi module failed!"));
    while (true);
  }

  WiFi.begin(_SSID, _PASS);
  Serial.print(F("Connecting to WiFi"));
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.println(F("Successfully connected to WiFi!"));
  delay(1000); // give some time for the connection to stabilize

  
}

void getSong(){
  String message = "https://api.getsong.co/tempo/?api_key=" + String(_GETSONGAPI) + "&bpm=" + String(bpm) + "&limit=1";

  http.begin(client, message, 443);
  http.setTimeout(5000);
  http.addHeader("User-Agent: Arduino UNO R4 WiFi");
  http.addHeader("Connection: close");

  int responseNum = http.GET();
  if (responseNum > 0) // OR if (responseNum == HTTP_CODE_OK) // 200 OK
  {
    // Get body
    String responseBody = http.getBody();
    //Serial.println(responseBody);
    Serial.println("Response code: " + String(responseNum));

    StaticJsonDocument<1024> doc;
    deserializeJson(doc, responseBody);

    song_title = doc["tempo"][0]["song_title"].as<String>();
    artist_name = doc["tempo"][0]["artist"]["name"].as<String>();
  } else {
    Serial.println("Request Failed: " + String(responseNum));
  }

  http.close();

  

}

void getYoutubeID(){
  String query = String(song_title) + " " + String(artist_name) + " official";
  query.replace(" ", "+");
  String message = "https://www.googleapis.com/youtube/v3/search?part=snippet&q=" + String(query) + "&type=video&maxResults=1&key=" + String(_YOUTUBEAPI);

  http.begin(client, message, 443);
  http.setTimeout(5000);
  http.addHeader("User-Agent: Arduino UNO R4 WiFi");
  http.addHeader("Connection: close");

  int responseNum = http.GET();
  if (responseNum > 0) // OR if (responseNum == HTTP_CODE_OK) // 200 OK
  {
    // Get body
    String responseBody = http.getBody();
    //Serial.println(responseBody);
    Serial.println("Response code: " + String(responseNum));

    StaticJsonDocument<1024> doc;
    deserializeJson(doc, responseBody);

    videoId = doc["items"][0]["id"]["videoId"].as<String>();
  } else {
    Serial.println("Request Failed: " + String(responseNum));
  }

  http.close();

  
}

void heartbeatUpdate(){
  if (pulseSensor.sawStartOfBeat()) {              
    bpm = pulseSensor.getBeatsPerMinute();
                                                   
    Serial.println("A HeartBeat Happened!");
    Serial.println(bpm); 
    
    updateScreen(bpm, previousPulse);


    previousPulse = bpm;
  }
}

void updateScreen(int currentBpm, int previousBpm){
  //Screen designed using lopaka.app
  u8g2.clearBuffer();	

  u8g2.setFontMode(1);
  u8g2.setBitmapMode(1);
  u8g2.drawRFrame(0, 0, 127, 64, 10);

  u8g2.setFont(u8g2_font_t0_15b_tr);
  u8g2.drawStr(52, 17, "BPM:");

  u8g2.setFont(u8g2_font_timR24_tr);

  //converts the change string into a char array
  String stringPulse = String(currentBpm);
  const char* arrayPulse = stringPulse.c_str();
  u8g2.drawStr(40, 45, arrayPulse);

  u8g2.drawBox(53, 50, 27, 10);

  u8g2.setDrawColor(2);
  u8g2.setFont(u8g2_font_6x10_tr);

  int change = currentBpm-previousBpm;
  String convertedString = String(change);
  if (change > 0){
    convertedString = "+"+convertedString;
  }

  //converts the change string into a char array  
  const char* arrayString = convertedString.c_str();
  u8g2.drawStr(56, 59, arrayString);

  u8g2.sendBuffer();
}



void loop()
{
  heartbeatUpdate();

  getSong();
  Serial.println("title: " + song_title);
  Serial.println("artist: " + artist_name);

  getYoutubeID();
  Serial.println("id:" + videoId);
  delay(5000);

}