/*
  =============================================================================
  Arduino R4 WiFi — LED Matrix with BLE-Controlled Animations
  =============================================================================

  This sketch demonstrates how to use the **onboard 12x8 LED matrix** on the
  Arduino Uno R4 WiFi to display **predefined animations** and switch between
  them dynamically using **Bluetooth Low Energy (BLE)**. Additionally, the
  onboard LED (LED_BUILTIN) reflects the BLE connection status: it turns ON
  when a BLE central device is connected and OFF when disconnected.

 **BLE Control:**
  - The Arduino advertises a BLE service with a single writable
    characteristic.
  - Writing a single byte ('0', '1', '2') to the characteristic
    switches the LED matrix animation in real-time.
      * '0' → Bouncing Ball
      * '1' → Heartbeat
      * '2' → DVD Logo

  - **Onboard LED Status Indicator:**
     - LED_BUILTIN lights up when a BLE central device is connected.
*/

#include <ArduinoBLE.h>

#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

// BLE service & characteristic
BLEService animService("19b10010-e8f2-537e-4f6c-d104768a1214");
BLECharacteristic animChar("19b10011-e8f2-537e-4f6c-d104768a1214",
                           BLERead | BLEWrite | BLENotify,
                           1  // 1 byte
);

char currentAnim = '0';  // default

void setup() {
  Serial.begin(115200);
  matrix.begin();

  pinMode(LED_BUILTIN, OUTPUT);

  // Initialize BLE
  if (!BLE.begin()) {
    Serial.println("BLE init failed!");
    while (1);
  }

  BLE.setLocalName("R4_Matrix");
  BLE.setAdvertisedService(animService);
  animService.addCharacteristic(animChar);
  BLE.addService(animService);

  // Write initial value
  uint8_t buf[1] = {(uint8_t)currentAnim};
  animChar.writeValue(buf, 1);

  BLE.advertise();
  Serial.println("BLE Matrix ready!");

  // Start default animation
  matrix.loadSequence(LEDMATRIX_ANIMATION_BOUNCING_BALL);
  matrix.play(true);
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected: ");
    Serial.println(central.address());
    digitalWrite(LED_BUILTIN, HIGH);

    while (central.connected()) {
      if (animChar.written()) {
        uint8_t val = animChar.value()[0];
        char newAnim = (char)val;

        if (newAnim != currentAnim) {  // switch if changed
          currentAnim = newAnim;
          Serial.print("Switching animation to: ");
          Serial.println(currentAnim);

          switch (currentAnim) {
            case '0':
              matrix.loadSequence(LEDMATRIX_ANIMATION_BOUNCING_BALL);
              break;
            case '1':
              matrix.loadSequence(LEDMATRIX_ANIMATION_HEARTBEAT);
              break;
            case '2':
              matrix.loadSequence(LEDMATRIX_ANIMATION_DVD);
              break;
            default:
              matrix.loadSequence(LEDMATRIX_ANIMATION_CLOUD);
              break;
          }
          matrix.play(true);  // loop
        }
      }
    }

    // Disconnected
    Serial.println("Central disconnected");
    digitalWrite(LED_BUILTIN, LOW);
  }
}