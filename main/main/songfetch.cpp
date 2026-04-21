#include <WiFiNINA.h>
#include <SPI.h>
#include "header.h"
#include "arduino_secrets.h"

char getSongApiKey[] = SECRET_GETSONG_API;

void fetchSongs(int bpm) {
  Serial.println("Fetching songs from GetSong...");

  WiFiClient apiClient;
  if (!apiClient.connect("api.getsong.co", 80)) {
    Serial.println("Connection to GetSong API failed!");
    return;
  }

  String path = "/tempo/?api_key=";
  path += getSongApiKey;
  path += "&bpm=";
  path += bpm;
  path += "&limit=";
  path += "1";

  apiClient.print("GET ");
  apiClient.print(path);
  apiClient.println(" HTTP/1.1");
  apiClient.println("Host: api.getsong.co");
  apiClient.println("Connection: close");
  apiClient.println();

  unsigned long timeout = millis();
  while (apiClient.available() == 0) {
    if (millis() - timeout > 10000) {
      Serial.println("GetSong API timeout!");
      apiClient.stop();
      return;
    }
  }

  String response = "";
  bool headersEnded = false;
  while (apiClient.available()) {
    String line = apiClient.readStringUntil('\n');
    if (!headersEnded) {
      if (line == "\r") headersEnded = true;
    } else {
      response += line;
    }
  }

  apiClient.stop();
  Serial.println(response);
  Song.title = "done";
  return;
}

