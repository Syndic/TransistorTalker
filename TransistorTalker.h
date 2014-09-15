#ifndef TRANSISTOR_TALKER_H
#define TRANSISTOR_TALKER_H

#include <Arduino.h>
#include "../Adafruit_VS1053_Library/Adafruit_VS1053.h"
#include "../ArduinoMap/ArduinoMap.h"
#include "../Flash/Flash.h"
#include "../RadioshackStrip/Radioshack_Strip.h"
#include <math.h>

#define PATTERN_MAP_CAPACITY 50

#define PATTERN_COLUMNS 2
#define BRIGHTNESS_INDEX 0
#define DURATION_INDEX 1

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
      Map<String, _FLASH_TABLE<uint16_t>*, PATTERN_MAP_CAPACITY>* allKnownPatterns,
      const uint16_t numberFilesFound,
      String filenamesFound[]);
  virtual void trigger();
private:
  bool talking;
  uint32_t color;
  RadioshackStrip& leds;
  Adafruit_VS1053_FilePlayer& filePlayer;
  Map<String, _FLASH_TABLE<uint16_t>*, PATTERN_MAP_CAPACITY> patterns;
  void playPattern(_FLASH_TABLE<uint16_t>* pattern);
};

#endif // TRANSISTOR_TALKER_H
