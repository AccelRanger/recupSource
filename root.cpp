// AccelSystems // LU18/05/25//

#include <HCSR04.h>

// Config

// Constants
const int enA = 11;
const int inA1 = 12;
const int inA2 = 13;

const int relay = 2;

// Variables
int valvePosition = 0;

// Joystick pins
const int VRx = A0;
const int VRy = A1;
const int SW = A2;

// Distance Sensor Pins

byte triggerPin = 21;
byte echoPin = 12;

// Functions

void killValveMotors() {
  digitalWrite(inA1, LOW);
  digitalWrite(inA2, LOW);
}

void setValveSpeed(int speed) {
  analogWrite(enA, speed);
}

void valve(String state) {
  Serial.print("returning valve");
  if (state == "open") {
    digitalWrite(inA1, HIGH);
    digitalWrite(inA2, LOW);
    valvePosition += 1;
    delay(500);
    killValveMotors();
  }
  else if (state == "close") {
    digitalWrite(inA1, LOW);
    digitalWrite(inA2, HIGH);
    valvePosition -= 1;
    delay(500);
    killValveMotors();
  }
}

void compressor(const char* state) {
  Serial.print("returning compressor");

  if (strcmp(state, "/") == 0) { // latching logic
    digitalWrite(relay, HIGH);
  }
  else if (strcmp(state, "//") == 0) {
    digitalWrite(relay, LOW);
  }
}

// Setup

void setup() {
  // Driver pins
  pinMode(enA, OUTPUT);
  pinMode(inA1, OUTPUT);
  pinMode(inA2, OUTPUT);
  
  // Relay pin
  pinMode(relay, OUTPUT);
  
  // Joystick setup
  pinMode(SW, INPUT_PULLUP);

  // Start serial communication
  Serial.begin(9600);

  // init libraries
  HCSR04.begin(triggerPin, echoPin);
  // init functions
  setValveSpeed(15);
  valve("open");
}

// Loop

void loop() {
  double* waterDistance = HCSR04.measureDistanceCm(); // distance
}
