#include <Arduino.h>
#include <U8g2lib.h>

//tf is all this
#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif
#include <PulseSensorPlayground.h>  // Includes the PulseSensorPlayground Library.

//  Variables
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
const int PulseWire = A0;      // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
int Threshold = 550;          // Determine which Signal to "count as a beat" and which to ignore.
                              // Use the "Gettting Started Project" to fine-tune Threshold Value beyond default setting.
                              // Otherwise leave the default "550" value.

PulseSensorPlayground pulseSensor;  // Creates an instance of the PulseSensorPlayground object called "pulseSensor"
int buzzerPIN = A1;

void setup() {

  Serial.begin(9600);


  pulseSensor.analogInput(PulseWire);
  pulseSensor.setThreshold(Threshold);
  //pulseSensor.blinkOnPulse(A1);

  // Double-check the "pulseSensor" object was created and "began" seeing a signal.
  if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object !");  // This prints one time at Arduino power-up,  or on Arduino reset.
  }

  u8g2.begin();
  //Oled.setFont(u8x8_font_amstrad_cpc_extended_r);
}


void loop() {

  if (pulseSensor.sawStartOfBeat()) {              // Constantly test to see if "a beat happened".
    u8g2.clearBuffer();	
    u8g2.setFont(u8g2_font_t0_22b_tf);
    int myBPM = pulseSensor.getBeatsPerMinute();   // Calls function on our pulseSensor object that returns BPM as an "int".
                                                   // "myBPM" hold this BPM value now.
    Serial.println("♥  A HeartBeat Happened ! ");  // If test is "true", print a message "a heartbeat happened".
    Serial.print("BPM: ");                         // Print phrase "BPM: "
    Serial.println(myBPM);   
    //Oled.print(myBPM);                      // Print the value inside of myBPM.
    char cstr[16];
    tone(buzzerPIN,330,500);
    itoa(myBPM, cstr, 10);
    u8g2.drawStr(40,40,cstr);
    u8g2.sendBuffer();
  }

  //delay(20);  // considered best practice in a simple sketch.
}
