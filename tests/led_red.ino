// code to make a strip of APA102 LEDs all red, for testing purposes
// this uses CO (clock) > connected to Pin 13 (SCK) and DO (data) > connected to
// Pin 11 (MOSI) power and ground  from a different source, with all grounds
// connected together

#include <FastLED.h>

#define NUM_LEDS 30
#define PIN_DATA 11
#define PIN_CLOCK 13

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<APA102, PIN_DATA, PIN_CLOCK, BGR>(leds, NUM_LEDS);
  FastLED.setBrightness(50);  // 0-255, start low
  FastLED.clear();
  FastLED.show();
}

void loop() {
  // all red
  fill_solid(leds, NUM_LEDS, CRGB::Red);
  FastLED.show();
}