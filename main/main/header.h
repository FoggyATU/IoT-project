#include <string>
#ifndef HEARTRATE_H
#define HEARTRATE_H

extern int heartRate;

struct {
  std::string title;
  std::string artist;
  std::string videoId;
} Song;

void fetchSongs(int bpm);


#endif