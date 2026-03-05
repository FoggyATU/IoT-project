#include <Arduino.h>
#include <U8g2lib.h> //Display library
#include <PulseSensorPlayground.h> //Pulse sensor library
#include <Wire.h> //i2c library

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); //Setup u8g2

const int pulse_PIN = A0;     // PulseSensor SIGNAL WIRE connected to ANALOG PIN 0
const int buzzer_PIN = A1;    // Buzzer connected to ANALOG PIN 1
const int volumeDown_PIN = 2; // Button connected to DIGITAL PIN 2
const int volumeUp_PIN = 3;   // Button connected to DIGITAL PIN 3

const int threshold = 550;    // Determine which Signal to "count as a beat" and which to ignore
const int maxVol = 600;
const int minVol = 100;

int previousPulse = 0;       
int currentVolume = 100;    

PulseSensorPlayground pulseSensor;

//im tired of writing comments

void setup() {
  Serial.begin(9600);

  pulseSensor.analogInput(pulse_PIN);
  pulseSensor.setThreshold(threshold);

  pinMode(volumeDown_PIN,INPUT_PULLUP);
  pinMode(volumeUp_PIN,INPUT_PULLUP);

  u8g2.begin();
}

void loop() {

  if (pulseSensor.sawStartOfBeat()) {              
    int myBPM = pulseSensor.getBeatsPerMinute();
                                                   
    Serial.println("A HeartBeat Happened!");
    Serial.println(myBPM); 
    
    updateScreen(myBPM, previousPulse);
    tone(buzzerPIN,currentVolume,500);

    previousPulse = myBPM
  }

  handleVolume();

  delay(20);
}

void handleVolume(){
  int volumeDown = digitalRead(volumeDown_PIN);
  int volumeUp = digitalRead(volumeUp_PIN);

  if (volumeDown == 0 && currentVolume > minVol){
    currentVolume -= 100;
  }

  if (volumeUp == 0 && currentVolume < maxVol){
    currentVolume += 100;
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
  u8g2.drawStr(40, 45, String(currentBpm));

  u8g2.drawBox(53, 50, 27, 10);

  u8g2.setDrawColor(2);
  u8g2.setFont(u8g2_font_6x10_tr);

  int change = currentBpm-previousBpm;
  String convertedString = String(change);
  if (change > 0){
    convertedString = "+"+convertedString
  }
  u8g2.drawStr(56, 59, convertedString);

  u8g2.sendBuffer();
}






