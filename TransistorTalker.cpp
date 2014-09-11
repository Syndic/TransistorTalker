#include "TransistorTalker.h"

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
