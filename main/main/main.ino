#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"
#include "header.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

int status = WL_IDLE_STATUS;

void setup() {
  Serial.begin(9600);

   while (status != WL_CONNECTED) {
    Serial.print("Connecting to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }
}

void loop() {
  fetchSongs(100);
  delay(1000);
  

}