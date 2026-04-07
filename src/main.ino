/*
  =============================================================================
  Arduino R4 WiFi — BLE Zone Controller with Audio (SoftwareSerial)
  =============================================================================
  BLE writes:
    '0' → all off,  matrix blank, play /MP3/0002.mp3
    '1' → ring 1 fire,  matrix shows "1", play /MP3/0001.mp3
    '2' → ring 2 fire,  matrix shows "2", play /MP3/0001.mp3
    '3' → ring 3 fire,  matrix shows "3", play /MP3/0001.mp3

  Wiring:
    3 × NeoPixel rings in series → Pin 6 (single data line)
    DFPlayer RX → Arduino D3 (TX) (via 1k resistor)
    DFPlayer TX → Arduino D2 (RX)

  SD Card Setup:
    Must contain a folder named "MP3"
    Files inside must be named "0001.mp3" and "0002.mp3"
=============================================================================*/

#include <ArduinoBLE.h>

#include "Arduino_LED_Matrix.h"  // <-- Moved BEFORE FastLED

// Kill the PORT macro to prevent conflicts with the R4 Matrix
#undef PORT

#include <FastLED.h>  // <-- Moved AFTER the Matrix and undef

// ── Audio config
// ───────────────────────────────────────────────────────────────
#include <SoftwareSerial.h>

#include "DFRobotDFPlayerMini.h"

// Set up SoftwareSerial on pins 2 (RX) and 3 (TX)
SoftwareSerial mySoftwareSerial(2, 3);
DFRobotDFPlayerMini myDFPlayer;

// ── Ring config
// ───────────────────────────────────────────────────────────────
#define RING_1_LEDS 12  // ← adjust to your 1st ring size
#define RING_2_LEDS 16  // ← adjust to your 2nd ring size
#define RING_3_LEDS 16  // ← adjust to your 3rd ring size

#define NUM_RINGS 3
#define NUM_LEDS (RING_1_LEDS + RING_2_LEDS + RING_3_LEDS)
#define PIN_DATA 6

// Arrays to keep track of each ring's size and where it starts on the strip
const int ringSizes[NUM_RINGS] = {RING_1_LEDS, RING_2_LEDS, RING_3_LEDS};
int ringOffsets[NUM_RINGS];  // We calculate these in setup()

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
// ───────────────────────────────────────────────────────────────
// Maps a 0-255 heat value to a fire colour (black → red → orange → yellow)
CRGB heatToColor(uint8_t t) {
  if (t < 85) return CRGB(t * 3, 0, 0);
  if (t < 170) return CRGB(255, (t - 85) * 3, 0);
  return CRGB(255, 255, (t - 170) * 3);
}

// Run one fire simulation step for the given ring index (0/1/2)
void fireStep(int ringIndex) {
  int base = ringOffsets[ringIndex];  // Where this ring starts in the array
  int ledsInRing =
      ringSizes[ringIndex];  // How many LEDs are in this specific ring

  // 1. Cool every cell down randomly
  for (int i = 0; i < ledsInRing; i++) {
    uint8_t cool = random(20, 55);
    heat[base + i] = (heat[base + i] > cool) ? heat[base + i] - cool : 0;
  }

  // 2. Diffuse heat around the ring
  for (int i = 0; i < ledsInRing; i++) {
    heat[base + i] = (heat[base + i] + heat[base + (i + 1) % ledsInRing] +
                      heat[base + (i + 2) % ledsInRing]) /
                     3;
  }

  // 3. Randomly ignite new sparks anywhere on the ring
  if (random(255) < 180) {
    int spark = random(ledsInRing);
    heat[base + spark] = qadd8(heat[base + spark], random(150, 255));
  }

  // 4. Convert heat → colour
  for (int i = 0; i < ledsInRing; i++) {
    leds[base + i] = heatToColor(heat[base + i]);
  }
}

// ── Helpers
// ───────────────────────────────────────────────────────────────────
void showMatrix(uint8_t bm[8][12]) { matrix.renderBitmap(bm, 8, 12); }

// Apply a new zone: update matrix, reset LEDs, trigger audio
void applyZone(char z) {
  FastLED.clear();
  memset(heat, 0, sizeof(heat));  // kill all fire state

  switch (z) {
    case '1':
      showMatrix(BM_1);
      myDFPlayer.playMp3Folder(1);  // Explicitly plays /MP3/0001.mp3
      break;
    case '2':
      showMatrix(BM_2);
      myDFPlayer.playMp3Folder(1);  // Explicitly plays /MP3/0001.mp3
      break;
    case '3':
      showMatrix(BM_3);
      myDFPlayer.playMp3Folder(1);  // Explicitly plays /MP3/0001.mp3
      break;
    default:  // Triggered on '0' or any invalid character
      showMatrix(BM_BLANK);
      myDFPlayer.playMp3Folder(2);  // Explicitly plays /MP3/0002.mp3
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
  mySoftwareSerial.begin(9600);  // Initialize SoftwareSerial for the DFPlayer

  // Calculate the offsets (starting positions) for each ring
  int currentOffset = 0;
  for (int i = 0; i < NUM_RINGS; i++) {
    ringOffsets[i] = currentOffset;
    currentOffset += ringSizes[i];
  }

  // Boot up Matrix & LEDs
  matrix.begin();
  pinMode(LED_BUILTIN, OUTPUT);

  FastLED.addLeds<WS2812B, PIN_DATA, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(180);
  FastLED.clear();
  FastLED.show();

  // Boot up BLE
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

  // Boot up DFPlayer
  delay(1000);  // Give the player a second to power on
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println("DFPlayer not detected! Check wiring.");
    // We intentionally don't freeze the code here so the LEDs still work even
    // if audio fails
  } else {
    Serial.println("DFPlayer ready.");
    myDFPlayer.volume(30);  // Cranked to 30! (Dial back if the board resets)
  }

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
        // Only trigger if the zone actually changed (prevents audio restarting
        // repeatedly)
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