#ifndef TRANSISTOR_TALKER_H
#define TRANSISTOR_TALKER_H

#include <Arduino.h>
#include "../RadioshackStrip/Radioshack_Strip.h"
#include "../Adafruit_VS1053_Library/Adafruit_VS1053.h"
#include "../ArduinoMap/ArduinoMap.h"
#include <math.h>

#define patternMapCapacity 30

struct Segment {
  uint8_t brightness;
  uint16_t msDuration;
};

struct Pattern {
  uint16_t segmentCount;
  Segment segments[];
};

class SdFileCollector {
public:
  SdFileCollector(uint8_t pin);
  uint16_t countFiles();
  void populateFilenameArray(String* array);
private:
  uint8_t pin;
  uint16_t countFilesInDirectory(File dir);
  void populateFilenameArray(String* array, File dir, uint16_t& index, String path);
};

class TransistorTalker {
public:
  TransistorTalker(
      uint32_t color,
      RadioshackStrip& leds,
      Adafruit_VS1053_FilePlayer& filePlayer);
  virtual ~TransistorTalker() {}
  virtual void configure(
      Map<String, Pattern, patternMapCapacity>& allKnownPatterns,
      const uint16_t numberFilesFound,
      String filenamesFound[]);
  virtual void trigger();
private:
  bool talking;
  uint32_t color;
  RadioshackStrip& leds;
  Adafruit_VS1053_FilePlayer& filePlayer;
  Map<String, Pattern, patternMapCapacity> patterns;
  void playPattern(Pattern& pattern);
};

#endif // TRANSISTOR_TALKER_H
