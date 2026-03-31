// Servo powered externally in 5V
// All grounds connected
// Servo signal pin connected to pin 9
// Send 'o' to open, 'c' to close

#include <Servo.h>

Servo myServo;

const int SERVO_PIN = 9;
const int LED_PIN = LED_BUILTIN;

const int POS_OPEN = 0;
const int POS_CLOSED = 90;
const int STEP_DELAY = 15;

void setup() {
  myServo.attach(SERVO_PIN);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);

  myServo.write(POS_CLOSED);
  Serial.println(
      "Lock ready. Starting position: closed. Send 'o' to open, 'c' to close.");
}

void loop() {
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    if (cmd == '\r' || cmd == '\n') return;  // ignore newline characters
    if (cmd == 'o')
      openLock();
    else if (cmd == 'c')
      closeLock();
  }
}

void openLock() {
  Serial.println("Opening...");
  moveServo(POS_OPEN);
  blinkLED(2, 100);
  Serial.println("Unlocked.");
}

void closeLock() {
  Serial.println("Closing...");
  moveServo(POS_CLOSED);
  blinkLED(1, 300);
  Serial.println("Locked.");
}

void moveServo(int target) {
  int current = myServo.read();
  int step = (target > current) ? 1 : -1;
  digitalWrite(LED_PIN, HIGH);
  for (int a = current; a != target; a += step) {
    myServo.write(a);
    delay(STEP_DELAY);
  }
  myServo.write(target);
  digitalWrite(LED_PIN, LOW);
}

void blinkLED(int times, int ms) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(ms);
    digitalWrite(LED_PIN, LOW);
    delay(ms);
  }
}