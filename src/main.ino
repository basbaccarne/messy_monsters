/*
  =============================================================================
  Arduino R4 WiFi — BLE Zone Controller
  =============================================================================
  BLE writes:
    '0' → all off,  matrix blank
    '1' → ring 1 fire,  matrix shows "1"
    '2' → ring 2 fire,  matrix shows "2"
    '3' → ring 3 fire,  matrix shows "3"

  Wiring: 3 × NeoPixel/WS2812 rings in series → Pin 6 (single data line)
=============================================================================*/

#include <ArduinoBLE.h>
#include <FastLED.h>

#include "Arduino_LED_Matrix.h"

// ── Ring config
// ───────────────────────────────────────────────────────────────
#define LEDS_PER_RING 12  // ← adjust to your ring size (8/12/16/24…)
#define NUM_RINGS 3
#define NUM_LEDS (LEDS_PER_RING * NUM_RINGS)
#define PIN_DATA 6

CRGB leds[NUM_LEDS];
uint8_t heat[NUM_LEDS];  // heat map for fire simulation

// ── Matrix
// ────────────────────────────────────────────────────────────────────
ArduinoLEDMatrix matrix;

// 12 × 8 bitmaps (col 0 = left, row 0 = top)
static uint8_t BM_BLANK[8][12] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

static uint8_t BM_1[8][12] = {
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0}, {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
};

static uint8_t BM_2[8][12] = {
    {0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}, {0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

static uint8_t BM_3[8][12] = {
    {0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}, {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
};

// ── BLE
// ───────────────────────────────────────────────────────────────────────
BLEService zoneService("19b10010-e8f2-537e-4f6c-d104768a1214");
BLECharacteristic zoneChar("19b10011-e8f2-537e-4f6c-d104768a1214",
                           BLERead | BLEWrite | BLENotify, 1);

char currentZone = '0';

// ── Fire effect
// ─────────────────────────────────────────────────────────────── Maps a 0-255
// heat value to a fire colour (black → red → orange → yellow)
CRGB heatToColor(uint8_t t) {
  if (t < 85) return CRGB(t * 3, 0, 0);
  if (t < 170) return CRGB(255, (t - 85) * 3, 0);
  return CRGB(255, 255, (t - 170) * 3);
}

// Run one fire simulation step for the given ring index (0/1/2)
void fireStep(int ringIndex) {
  int base = ringIndex * LEDS_PER_RING;

  // 1. Cool every cell down randomly
  for (int i = 0; i < LEDS_PER_RING; i++) {
    uint8_t cool = random(20, 55);
    heat[base + i] = (heat[base + i] > cool) ? heat[base + i] - cool : 0;
  }

  // 2. Diffuse heat around the ring
  for (int i = 0; i < LEDS_PER_RING; i++) {
    heat[base + i] = (heat[base + i] + heat[base + (i + 1) % LEDS_PER_RING] +
                      heat[base + (i + 2) % LEDS_PER_RING]) /
                     3;
  }

  // 3. Randomly ignite new sparks anywhere on the ring
  if (random(255) < 180) {
    int spark = random(LEDS_PER_RING);
    heat[base + spark] = qadd8(heat[base + spark], random(150, 255));
  }

  // 4. Convert heat → colour
  for (int i = 0; i < LEDS_PER_RING; i++) {
    leds[base + i] = heatToColor(heat[base + i]);
  }
}

// ── Helpers
// ───────────────────────────────────────────────────────────────────
void showMatrix(uint8_t bm[8][12]) { matrix.renderBitmap(bm, 8, 12); }

// Apply a new zone: update matrix + reset LEDs/heat
void applyZone(char z) {
  FastLED.clear();
  memset(heat, 0, sizeof(heat));  // kill all fire state

  switch (z) {
    case '1':
      showMatrix(BM_1);
      break;
    case '2':
      showMatrix(BM_2);
      break;
    case '3':
      showMatrix(BM_3);
      break;
    default:
      showMatrix(BM_BLANK);
      break;
  }
  FastLED.show();
}

// Called every loop iteration – drives whichever ring is active
void runFire() {
  int ring = currentZone - '1';  // '1'→0, '2'→1, '3'→2, anything else → -1
  if (ring >= 0 && ring < NUM_RINGS) {
    fireStep(ring);
    FastLED.show();
    delay(30);
  }
}

// ── Setup / Loop
// ──────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  matrix.begin();
  pinMode(LED_BUILTIN, OUTPUT);

  FastLED.addLeds<WS2812B, PIN_DATA, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(180);
  FastLED.clear();
  FastLED.show();

  if (!BLE.begin()) {
    Serial.println("BLE init failed!");
    while (1);
  }

  BLE.setLocalName("R4_Zones");
  BLE.setAdvertisedService(zoneService);
  zoneService.addCharacteristic(zoneChar);
  BLE.addService(zoneService);

  uint8_t buf[1] = {(uint8_t)currentZone};
  zoneChar.writeValue(buf, 1);

  BLE.advertise();
  showMatrix(BM_BLANK);
  Serial.println("BLE Zone controller ready!");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected: ");
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, HIGH);

    while (central.connected()) {
      if (zoneChar.written()) {
        char newZone = (char)zoneChar.value()[0];
        if (newZone != currentZone) {
          currentZone = newZone;
          Serial.print("Zone → ");
          Serial.println(currentZone);
          applyZone(currentZone);
        }
      }
      runFire();  // keeps animating while connected
    }

    Serial.println("Disconnected");
    digitalWrite(LED_BUILTIN, LOW);
  }

  runFire();  // also animates while waiting for a connection
}