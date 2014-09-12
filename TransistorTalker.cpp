#include "TransistorTalker.h"

SdFileCollector::SdFileCollector(uint8_t pin) : pin(pin) {
  if (!SD.begin(pin)) {
    Serial.println(F("SD failed, or not present"));
    while (1);  // don't do anything more
  }
  Serial.println("SD OK!");
}

uint16_t SdFileCollector::countFiles() {
  return countFilesInDirectory(SD.open("/"));
}

uint16_t SdFileCollector::countFilesInDirectory(File dir) {
  int count = 0;
  File entry =  dir.openNextFile();
  while(entry) {
    if (entry.isDirectory()) { count += countFilesInDirectory(entry); }
    else { count++; }

    entry.close();
    entry = dir.openNextFile();
  }
  return count;
}

void SdFileCollector::populateFilenameArray(String* array) {
  uint16_t index = 0;
  populateFilenameArray(array, SD.open("/"), index, String(""));
}

void SdFileCollector::populateFilenameArray(String* array, File dir, uint16_t& index, String path) {
  File entry =  dir.openNextFile();
  while(entry) {
    if (entry.isDirectory()) {
      populateFilenameArray(array, entry, index, String(path + entry.name() + "/"));
    } else { array[index++] = String(path + entry.name()); }

    entry.close();
    entry =  dir.openNextFile();
  }
}


TransistorTalker::TransistorTalker(
    uint32_t color,
    RadioshackStrip& leds,
    Adafruit_VS1053_FilePlayer& filePlayer)
  : talking(false),
    color(color),
    leds(leds),
    filePlayer(filePlayer),
    patterns(Map<String, Pattern, patternMapCapacity>())
{}

void TransistorTalker::configure(
    Map<String, Pattern, patternMapCapacity>& allKnownPatterns,
    const uint16_t numberFilesFound,
    String filenamesFound[]) {
  patterns = Map<String, Pattern, patternMapCapacity>();
  
  for(int i=0; i<numberFilesFound; ++i) {
    uint16_t index = allKnownPatterns.indexOf(filenamesFound[i]);
    if(index != -1) {
      patterns[allKnownPatterns.keyAt(index)] = allKnownPatterns.valueAt(index);
    }
  }
}

void TransistorTalker::trigger() {
  uint8_t oldSREG = SREG; // Store interrupt register
  noInterrupts();
  if(talking || patterns.size() == 0) {
    SREG = oldSREG; // restore interrupt register
    return;
  }
  talking = true;
  interrupts();
  
  uint16_t selected = random(patterns.size());
  String filename = patterns.keyAt(selected);
  Pattern pattern = patterns.valueAt(selected);
  
  uint8_t length = filename.length()+1;
  char buffer[length];
  filename.toCharArray(buffer, length);
  
  filePlayer.playFullFile(buffer);
  playPattern(pattern);
  
  talking = false;
  SREG = oldSREG; // restore interrupt register
}

void TransistorTalker::playPattern(Pattern& pattern) {
  Serial.println("I'm outputting a pattern!");
  
  for(int i = 0; i<pattern.segmentCount; ++i) {
    leds.setBrightness(pattern.segments[i].brightness);
    for (int i = 0; i<leds.numPixels(); ++i) {
      leds.setPixelColor(i, color);
    }
    leds.show();
    
    delay(pattern.segments[i].msDuration);
  }
}
