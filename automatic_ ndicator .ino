#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <WiFi.h>

TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

#define LEFT_LED 22
#define RIGHT_LED 23

float prevCourse = 0;
bool firstFix = true;
int turnDirection = 0; // 1 = Right, -1 = Left
bool blinking = false;
int blinkCount = 0;
unsigned long lastBlinkTime = 0;
bool ledState = false;

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);

  // Initialize GPS UART on Pins 16 (RX) and 17 (TX)
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);

  // Set GPS update rate to 2Hz using UBX command
  const byte rate2Hz[] = {0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 0xF4, 0x01, 0x01, 0x00, 0x01, 0x00, 0x0B, 0x77};
  for (int i = 0; i < sizeof(rate2Hz); i++) {
    gpsSerial.write(rate2Hz[i]);
  }

  pinMode(LEFT_LED, OUTPUT);
  pinMode(RIGHT_LED, OUTPUT);
  digitalWrite(LEFT_LED, LOW);
  digitalWrite(RIGHT_LED, LOW);

  Serial.println("SYSTEM READY - OFFLINE LOW POWER MODE");
}

void startBlinking(int dir) {
  blinking = true;
  blinkCount = 0;
  ledState = false;
  turnDirection = dir;
  lastBlinkTime = millis();
}

void stopBlinking() {
  blinking = false;
  turnDirection = 0;
  digitalWrite(LEFT_LED, LOW);
  digitalWrite(RIGHT_LED, LOW);
}

void loop() {
  while (gpsSerial.available() > 0) {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isValid() && gps.course.isUpdated()) {
    float currentSpeed = gps.speed.kmph();
    float currentCourse = gps.course.deg();

    if (firstFix) {
      prevCourse = currentCourse;
      firstFix = false;
    } else {
      float diff = currentCourse - prevCourse;
      
      // Handle 0-360 degree wrap-around
      if (diff > 180.0) diff -= 360.0;
      if (diff < -180.0) diff += 360.0;

      // Detect turning threshold if vehicle is moving (> 1 km/h)
      if (!blinking && currentSpeed > 1.0) {
        if (diff > 8.0) {
          Serial.println(">>> RIGHT TURN");
          startBlinking(1);
          prevCourse = currentCourse;
        } else if (diff < -8.0) {
          Serial.println("<<< LEFT TURN");
          startBlinking(-1);
          prevCourse = currentCourse;
        }
      }

      if (!blinking) {
        prevCourse = currentCourse;
      }
    }
  }

  // Blinking LED logic (Non-blocking)
  if (blinking) {
    if (millis() - lastBlinkTime > 300) {
      lastBlinkTime = millis();
      ledState = !ledState;

      if (turnDirection == 1) {
        digitalWrite(RIGHT_LED, ledState);
        digitalWrite(LEFT_LED, LOW);
      } else if (turnDirection == -1) {
        digitalWrite(LEFT_LED, ledState);
        digitalWrite(RIGHT_LED, LOW);
      }

      if (ledState == HIGH) {
        blinkCount++;
      }

      // Stop blinking after 10 flashes
      if (blinkCount >= 10) {
        stopBlinking();
      }
    }
  }
}
