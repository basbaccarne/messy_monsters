/*
 * Arduino UNO R4 WiFi — LED Matrix Predefined Animations
 * -------------------------------------------------------
 * LEDMATRIX_ANIMATION_BOUNCING_BALL  → A ball bouncing around
 * LEDMATRIX_ANIMATION_STARTUP        → Startup splash animation
 * LEDMATRIX_ANIMATION_HEARTBEAT      → Pulsing heart
 * LEDMATRIX_ANIMATION_WIFI_SEARCH    → Wi-Fi scanning animation
 * LEDMATRIX_ANIMATION_ARROWS_COMPASS → Rotating compass arrows
 * LEDMATRIX_ANIMATION_AUDIO_WAVEFORM → Audio waveform bars
 * LEDMATRIX_ANIMATION_SPINNING_COIN  → A coin spinning
 * LEDMATRIX_ANIMATION_DVD            → DVD logo bouncing
 * LEDMATRIX_ANIMATION_EXPLOSION      → Explosion burst
 * LEDMATRIX_ANIMATION_TETRIS_INTRO   → Tetris falling blocks
 * LEDMATRIX_ANIMATION_SPACESHIP      → Flying spaceship
 * LEDMATRIX_ANIMATION_EULERPATH      → Euler path drawing
 * LEDMATRIX_ANIMATION_INFINITY       → Infinity loop
 * LEDMATRIX_ANIMATION_SNAKE          → Snake game loop
 * LEDMATRIX_ANIMATION_CLOUD          → Cloud pattern
 */

#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

void setup() {
  matrix.begin();

  // Load and play a built-in animation
  matrix.loadSequence(LEDMATRIX_ANIMATION_HEARTBEAT);
  matrix.play(true);  // true = loop forever
}

void loop() {
  // nothing needed — animation runs automatically
}