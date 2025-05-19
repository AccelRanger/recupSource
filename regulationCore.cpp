// AccelSystems // LU19/05/25//

#include <HCSR04.h>

// Config

// Constants
const int enAV = 11;
const int inA1V = 12;
const int inA2V = 13;
const int relay = 2;

// Distance Sensor Pins
byte triggerPin = 21;
byte echoPin = 10;

// Variables
int valvePosition = 0;
int compressorPosition = 0;

// Regulation // !!!! CONFIG !!!!
const int ValveTime = 500;
const int MaxLength = 13; // cm
const int MinLength = 5;  // cm

// Functions

void killValveMotors() {
  digitalWrite(inA1V, LOW);
  digitalWrite(inA2V, LOW);
}

void setValveSpeed(int speed) {
  analogWrite(enAV, speed);
}

void valve(String state, int duration) {
  Serial.println("returning valve");
  if (state == "open") {
    digitalWrite(inA1V, HIGH);
    digitalWrite(inA2V, LOW);
    valvePosition = 1;
    delay(duration);
    killValveMotors();
  } else if (state == "close") {
    digitalWrite(inA1V, LOW);
    digitalWrite(inA2V, HIGH);
    valvePosition = 0;
    delay(duration);
    killValveMotors();
  }
}

void compressor(bool enable) {
  Serial.println("returning compressor");
  
  if (enable) {
    compressorPosition = 1;
  } else {
    compressorPosition = 0;
  }

  digitalWrite(relay, enable ? HIGH : LOW);
}

// Setup

void setup() {
  pinMode(enAV, OUTPUT);
  pinMode(inA1V, OUTPUT);
  pinMode(inA2V, OUTPUT);
  pinMode(relay, OUTPUT);

  Serial.begin(9600);
  HCSR04.begin(triggerPin, echoPin);

  setValveSpeed(38); // 255max /  2.55 = 1% / 38 = 15%
}

// Loop

void loop() {
  double* waterDistance = HCSR04.measureDistanceCm(); // waterDistance[0]

  if (waterDistance[0] >= MaxLength) {
    if (compressorPosition == 1) {
      compressor(false);
    } else if (valvePosition == 0) {
      valve("open", ValveTime);
    }
  }
  else if (waterDistance[0] <= MinLength) {
    if (compressorPosition == 0) {
      compressor(true);
    } else if (valvePosition == 1) {
      valve("close", ValveTime);
    }
  }
}
