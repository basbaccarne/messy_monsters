// Simple NeoPixel chase test for 8-pixel stick
//
// 🔌 WIRING
//
// 📡 DATA LINE:
// Arduino Pin 6 ──[330–470 Ω]──> DIN (NeoPixel)
// • Resistor Protects the first LED from signal spikes
//
// ⚡ POWER:
// • Use a stable 5V EXTERNAL power supply
// • 5V → +5V, GND → GND
//
// 🔋 CAPACITOR:
// • Add 470–1000 µF between 5V and GND (close to LEDs)
// • This smooths power dips (prevents flicker/glitches)
//
// • Polarity matters:
//   ➕ Long leg = + → connect to 5V
//   ➖ Short leg = – → connect to GND
//   ⚠️ Stripe on the capacitor = NEGATIVE (–)
//
// 🔗 GROUND:
// Connect all grounds

#include <Adafruit_NeoPixel.h>

const int ledPin = 6;
const int N_LEDs = 12;

Adafruit_NeoPixel strip =
    Adafruit_NeoPixel(N_LEDs, ledPin, NEO_GRB + NEO_KHZ800);

void setup() { strip.begin(); }

void loop() {
  chase(strip.Color(255, 0, 0));  // Red
  chase(strip.Color(0, 255, 0));  // Green
  chase(strip.Color(0, 0, 255));  // Blue
}

static void chase(uint32_t c) {
  for (uint16_t i = 0; i < strip.numPixels() + 4; i++) {
    strip.setPixelColor(i, c);
    strip.setPixelColor(i - 4, 0);
    strip.show();
    delay(25);
  }
}