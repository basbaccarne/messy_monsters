// This is a quick example of how to use the DFRobot DFPlayer Mini MP3 module
// with an Arduino Uno. It plays a random track from the SD card every 3
// seconds.

// Wires (Arduino Uno):
// Arduino D2 (later: RX) to DF player TX
// Arduino D3 (later: TX) (via 1k resistor) to DF player RX
// Arduino GND to DF player GND
// Arduino 5V to DF player VCC
// Speaker - to DF player SPK1
// Speaker + to DF player SPK2

// Pinout

//  VCC    ) --- \__/ --- ( BUSY
//  RX     ) -          - ( USB-
//  TX     ) -          - ( USB+
//  DAC_R  ) -          - ( ADKEY_2
//  DAC_L  ) -          - ( ADKEY_1
//  SPK_1  ) -          - ( IO_2
//  GND    ) -          - ( GND
//  SPK_2  ) -          - ( IO_2
//         ----------------

// file structure in the SD card:
// Create a folder called MP3 in the root folder
// files need to start with 4 digits: e.g. 0001.mp3, 0002.mp3, 0003.mp3, etc.
// after these 4 digits, the file name can be anything: e.g. 0001.mp3,
// 0001_hello.mp3, 0001_hello_world.mp3, etc.

// to use ffmpeg to convert to mp3: ffmpeg -i 0001.wav -vn -ar 44100 -ac 2 -b:a
// 192k 0001.mp3

#include <SoftwareSerial.h>

#include "DFRobotDFPlayerMini.h"

SoftwareSerial AudioSerial(2, 3);  // RX, TX
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  Serial.begin(115200);
  AudioSerial.begin(9600);

  randomSeed(analogRead(A0));  // random seed for track selection

  if (!myDFPlayer.begin(AudioSerial)) {
    Serial.println("DFPlayer not detected!");
    while (true);  // stop execution
  }

  myDFPlayer.volume(5);  // From 0 to 30
}

void loop() {
  static unsigned long timer = millis();
  const int n_audiofiles = 10;

  if (millis() - timer > 3000) {
    timer = millis();
    int number = random(1, n_audiofiles + 1);
    myDFPlayer.play(number);
    Serial.print("Playing track: ");
    Serial.println(number);
  }
}