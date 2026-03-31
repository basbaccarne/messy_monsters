#include <FastLED.h>

#define NUM_LEDS 30
#define PIN_DATA 11
#define PIN_CLOCK 13

CRGB leds[NUM_LEDS];

uint8_t brightness = 50;
uint8_t hueOffset = 0;
bool rainbow = true;
CRGB solidColor = CRGB::White;

void setup() {
  FastLED.addLeds<APA102, PIN_DATA, PIN_CLOCK, BGR>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);
  FastLED.clear();
  FastLED.show();
  Serial.begin(9600);
  printHelp();
}

void loop() {
  handleSerial();

  if (rainbow) {
    fill_rainbow(leds, NUM_LEDS, hueOffset, 256 / NUM_LEDS);
    hueOffset++;
    FastLED.show();
    delay(20);
  }
}

void handleSerial() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input == "rainbow") {
      rainbow = true;
      Serial.println("Mode: rainbow");

    } else if (input.startsWith("color ")) {
      // expects: color R G B  e.g. "color 255 0 0"
      int r, g, b;
      sscanf(input.c_str(), "color %d %d %d", &r, &g, &b);
      solidColor = CRGB(r, g, b);
      rainbow = false;
      fill_solid(leds, NUM_LEDS, solidColor);
      FastLED.show();
      Serial.print("Color set to RGB(");
      Serial.print(r);
      Serial.print(", ");
      Serial.print(g);
      Serial.print(", ");
      Serial.print(b);
      Serial.println(")");

    } else if (input.startsWith("brightness ")) {
      int val;
      sscanf(input.c_str(), "brightness %d", &val);
      brightness = constrain(val, 0, 255);
      FastLED.setBrightness(brightness);
      FastLED.show();
      Serial.print("Brightness: ");
      Serial.println(brightness);

    } else if (input == "off") {
      rainbow = false;
      FastLED.clear();
      FastLED.show();
      Serial.println("Off.");

    } else if (input == "help") {
      printHelp();
    }
  }
}

void printHelp() {
  Serial.println("--- LED Strip Commands ---");
  Serial.println("rainbow           → rainbow animation");
  Serial.println("color R G B       → solid color e.g. 'color 255 0 0'");
  Serial.println("brightness 0-255  → set brightness");
  Serial.println("off               → turn off");
  Serial.println("help              → show this");
}