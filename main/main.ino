#include <Arduino.h>
#include <U8g2lib.h>


#include <Wire.h>
#include <PulseSensorPlayground.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); //Setup u8g2

const int pulse_PIN = A0;     // PulseSensor SIGNAL WIRE connected to ANALOG PIN 0

int threshold = 550;    // Determine which Signal to "count as a beat" and which to ignore

int previousPulse = 0;        

PulseSensorPlayground pulseSensor;

//im tired of writing comments

void setup() {
  Serial.begin(9600);

  pulseSensor.analogInput(pulse_PIN);
  pulseSensor.setThreshold(threshold);
  //pulseSensor.blinkOnPulse(buzzer_PIN);
  pulseSensor.begin();

  u8g2.begin();
}

void loop() {

  if (pulseSensor.sawStartOfBeat()) {              
    int myBPM = pulseSensor.getBeatsPerMinute();
                                                   
    Serial.println("A HeartBeat Happened!");
    Serial.println(myBPM); 
    
    updateScreen(myBPM, previousPulse);
    
    Serial.println("Current Volume:");
    Serial.println(currentVolume);

    previousPulse = myBPM;
  }


  delay(100);
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






