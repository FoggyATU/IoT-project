#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h"

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
char getSongApiKey[] = SECRET_GETSONG_API;
char youtubeApiKey[] = SECRET_YOUTUBE_API;

int status = WL_IDLE_STATUS;
WiFiServer server(80);

struct Song {
  char title[64];
  char artist[64];
  char songId[32];
  char videoId[16];  // YouTube video ID (always 11 chars)
};

Song songs[5];
int songCount = 0;
int targetBpm = 127;

void setup() {
  Serial.begin(9600);

 /*
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("WiFi module failed!");
    while (true);
  }
  */

  while (status != WL_CONNECTED) {
    Serial.print("Connecting to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }

  Serial.println("Connected!");
  printWifiStatus();

  fetchSongs(targetBpm, 5);

  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  WiFiClient webClient = server.available();
  if (webClient) {
    Serial.println("Client connected");
    boolean currentLineIsBlank = true;
    String request = "";

    while (webClient.connected()) {
      if (webClient.available()) {
        char c = webClient.read();
        request += c;

        if (c == '\n' && currentLineIsBlank) {
          if (request.indexOf("GET /refresh") >= 0) {
            fetchSongs(targetBpm, 5);
          }

          webClient.println("HTTP/1.1 200 OK");
          webClient.println("Content-Type: text/html");
          webClient.println("Connection: close");
          webClient.println();
          webClient.println("<!DOCTYPE HTML><html>");
          webClient.println("<head><meta charset='UTF-8'><title>BPM Songs</title></head>");
          webClient.println("<body>");
          webClient.print("<h2>Songs at ");
          webClient.print(targetBpm);
          webClient.println(" BPM</h2>");
          webClient.println("<a href='/refresh'>Refresh Songs</a><br><br>");
          webClient.println("<table border='1' cellpadding='6'>");
          webClient.println("<tr><th>Song ID</th><th>Title</th><th>Artist</th><th>YouTube</th></tr>");

          for (int i = 0; i < songCount; i++) {
            webClient.print("<tr><td>");
            webClient.print(songs[i].songId);
            webClient.print("</td><td>");
            webClient.print(songs[i].title);
            webClient.print("</td><td>");
            webClient.print(songs[i].artist);
            webClient.print("</td><td>");
            if (strlen(songs[i].videoId) > 0) {
              webClient.print("<a href='https://youtu.be/");
              webClient.print(songs[i].videoId);
              webClient.print("' target='_blank'>Watch</a>");
            } else {
              webClient.print("N/A");
            }
            webClient.println("</td></tr>");
          }

          webClient.println("</table></body></html>");
          break;
        }

        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }

    delay(1);
    webClient.stop();
    Serial.println("Client disconnected");
  }
}

void fetchSongs(int bpm, int limit) {
  Serial.println("Fetching songs from GetSong...");
  songCount = 0;

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
  path += limit;

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
  parseAndStoreSongs(response);

  // Now fetch YouTube ID for each song
  for (int i = 0; i < songCount; i++) {
    fetchYoutubeId(i);
    delay(500); // small delay between API calls to be safe
  }
}

void fetchYoutubeId(int songIndex) {
  Serial.print("Fetching YouTube ID for: ");
  Serial.println(songs[songIndex].title);

  WiFiSSLClient youtubeClient;
  if (!youtubeClient.connect("www.googleapis.com", 443)) {
    Serial.println("Connection to YouTube API failed!");
    return;
  }

  // Build query: "Song Title Artist official"
  String query = String(songs[songIndex].title) + " " + String(songs[songIndex].artist) + " official";

  // URL encode spaces as %20
  query.replace(" ", "%20");

  String path = "/youtube/v3/search?part=snippet&q=";
  path += query;
  path += "&type=video&maxResults=1&key=";
  path += youtubeApiKey;

  youtubeClient.print("GET ");
  youtubeClient.print(path);
  youtubeClient.println(" HTTP/1.1");
  youtubeClient.println("Host: www.googleapis.com");
  youtubeClient.println("Connection: close");
  youtubeClient.println();

  unsigned long timeout = millis();
  while (youtubeClient.available() == 0) {
    if (millis() - timeout > 10000) {
      Serial.println("YouTube API timeout!");
      youtubeClient.stop();
      return;
    }
  }

  String response = "";
  bool headersEnded = false;
  while (youtubeClient.available()) {
    String line = youtubeClient.readStringUntil('\n');
    if (!headersEnded) {
      if (line == "\r") headersEnded = true;
    } else {
      response += line;
    }
  }

  youtubeClient.stop();

  // Parse videoId from response
  // YouTube returns: "videoId": "xxxxxxxxxxx"
  int vidIdx = response.indexOf("\"videoId\"");
  if (vidIdx >= 0) {
    int vidStart = response.indexOf("\"", vidIdx + 10) + 1;
    int vidEnd = response.indexOf("\"", vidStart);
    String videoId = response.substring(vidStart, vidEnd);
    videoId.toCharArray(songs[songIndex].videoId, 16);
    Serial.print("Found video ID: ");
    Serial.println(songs[songIndex].videoId);
  } else {
    Serial.println("No video ID found.");
    songs[songIndex].videoId[0] = '\0';
  }
}

void parseAndStoreSongs(String json) {
  int searchFrom = 0;

  while (songCount < 5) {
    int titleIdx = json.indexOf("\"song_title\"", searchFrom);
    if (titleIdx < 0) break;
    int titleStart = json.indexOf("\"", titleIdx + 13) + 1;
    int titleEnd = json.indexOf("\"", titleStart);

    int artistIdx = json.indexOf("\"name\"", titleIdx);
    if (artistIdx < 0) break;
    int artistStart = json.indexOf("\"", artistIdx + 7) + 1;
    int artistEnd = json.indexOf("\"", artistStart);

    int idIdx = json.indexOf("\"id\"", titleIdx);
    if (idIdx < 0) break;
    int idStart = json.indexOf("\"", idIdx + 5) + 1;
    int idEnd = json.indexOf("\"", idStart);

    json.substring(titleStart, titleEnd).toCharArray(songs[songCount].title, 64);
    json.substring(artistStart, artistEnd).toCharArray(songs[songCount].artist, 64);
    json.substring(idStart, idEnd).toCharArray(songs[songCount].songId, 32);
    songs[songCount].videoId[0] = '\0'; // clear video ID until fetched

    Serial.print("Parsed: ");
    Serial.print(songs[songCount].title);
    Serial.print(" by ");
    Serial.println(songs[songCount].artist);

    songCount++;
    searchFrom = idEnd;
  }

  Serial.print(songCount);
  Serial.println(" songs parsed.");
}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  long rssi = WiFi.RSSI();
  Serial.print("Signal (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
}