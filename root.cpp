// AccelSystems // LU18/05/25//

#include <HCSR04.h>

// Config

// Constants
const int enAV = 11;
const int inA1V = 12;
const int inA2V = 13;
const int relay = 2;

// Main Motors
const int enA = 9;
const int inA1 = 8;
const int inA2 = 7;
const int enB = 3;
const int inB3 = 5;
const int inB4 = 4;

// Variables
int valvePosition = 0;

// Joystick pins
const int VRx = A0;
const int VRy = A1;
const int SW = A2;

// Distance Sensor Pins
byte triggerPin = 21;
byte echoPin = 10;

// Regulation
const int MaxTime = 500;
const int MinTime = 50;
const int MaxLength = 13; // cm
const int MinLength = 5;  // cm

const int midSpotReg = (MaxLength + MinLength) / 2;
const int MidTimeReg = (MaxTime + MinTime) / 2;
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
    valvePosition += 1;
    delay(duration);
    killValveMotors();
  } else if (state == "close") {
    digitalWrite(inA1V, LOW);
    digitalWrite(inA2V, HIGH);
    valvePosition -= 1;
    delay(duration);
    killValveMotors();
  }
}

void motorControl(char motor, char direction) {
  stop_motors();

  if (motor == 'A') {
    if (direction == 'L') {
      digitalWrite(inA1, HIGH);
      digitalWrite(inA2, LOW);
    } else if (direction == 'R') {
      digitalWrite(inA1, LOW);
      digitalWrite(inA2, HIGH);
    }
  }

  else if (motor == 'B') {
    if (direction == 'L') {
      digitalWrite(inB3, HIGH);
      digitalWrite(inB4, LOW);
    } else if (direction == 'R') {
      digitalWrite(inB3, LOW);
      digitalWrite(inB4, HIGH);
    }
  }
}

void stop_motors() {
  digitalWrite(inA1, LOW);
  digitalWrite(inA2, LOW);
  digitalWrite(inB3, LOW);
  digitalWrite(inB4, LOW);
}

void compressor(bool enable) {
  Serial.println("returning compressor");
  digitalWrite(relay, enable ? HIGH : LOW);
}

// joystick control

void joystickControl() {
  int xValue = analogRead(VRx);
  int yValue = analogRead(VRy);
  bool buttonPressed = digitalRead(SW) == LOW;

 // Serial.print("X: "); Serial.print(xValue);
 // Serial.print(" | Y: "); Serial.print(yValue);
 // Serial.print(" | Button: "); Serial.println(buttonPressed);

  // speed set
  analogWrite(enA, 255);
  analogWrite(enB, 255);

  // kill motors
  stop_motors();

  int margin = 100;

  if (xValue < margin && abs(yValue - 519) < margin) {
    motorControl('A', 'L');  // Move Motor A forward
    motorControl('B', 'R');  // Move Motor B backward
  } 
  else if (xValue > 1023 - margin && abs(yValue - 519) < margin) {
    motorControl('A', 'R');  // Move Motor A backward
    motorControl('B', 'L');  // Move Motor B forward
  } 
  else if (abs(xValue - 519) < margin && yValue > 1023 - margin) {
    motorControl('A', 'R');  // Rotate Motor A backward
    motorControl('B', 'R');  // Rotate Motor B backward
  } 
  else if (abs(xValue - 519) < margin && yValue < margin) {
    motorControl('A', 'L');  // Rotate Motor A forward
    motorControl('B', 'L');  // Rotate Motor B forward
  } 
  else if (abs(xValue - 519) < margin && abs(yValue - 519) < margin) {
    stop_motors();  // Neutral position
  }
}


// Setup

void setup() {
  pinMode(enAV, OUTPUT);
  pinMode(inA1V, OUTPUT);
  pinMode(inA2V, OUTPUT);
  pinMode(relay, OUTPUT);
  pinMode(SW, INPUT_PULLUP);

  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(inA1, OUTPUT);
  pinMode(inA2, OUTPUT);
  pinMode(inB3, OUTPUT);
  pinMode(inB4, OUTPUT);

  Serial.begin(9600);
  HCSR04.begin(triggerPin, echoPin);

  setValveSpeed(15);
}

// Loop

unsigned long previousJoystickTime = 0;
unsigned long previousRegulationTime = 0;
const int joystickInterval = 50;   // Joystick delay interval
const int regulationInterval = 500; // Valve delay regulation

void loop() {
  unsigned long currentTime = millis(); // get time async

  if (currentTime - previousJoystickTime >= joystickInterval) { // own loop for joystick
    previousJoystickTime = currentTime;
    joystickControl();
  }

  if (currentTime - previousRegulationTime >= regulationInterval) {  // own loop for regulation
    previousRegulationTime = currentTime;

    double* waterDistance = HCSR04.measureDistanceCm();
    double distance = *waterDistance;
    int waterDistanceV= static_cast<int>(distance);

    Serial.print("Water Level: ");
    Serial.print(waterDistanceV);

    compressor(waterDistance < midSpotReg); // True/False latching logic for compressor

    if (waterDistance < MinLength) {
      valve("open", MidTimeReg);
    } else if (waterDistance > MaxLength) {
      valve("close", MidTimeReg);
    } else {
      killValveMotors(); // Balancing
    }
  }
}
