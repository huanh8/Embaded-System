#include "AutonomousRobot.h"

AutonomousRobot::AutonomousRobot(int trigPin, int echoPin, int switchPin, int AIN1, int AIN2, int PWMA, int PWMB, int BIN1, int BIN2) {
  this->trigPin = trigPin;
  this->echoPin = echoPin;
  this->switchPin = switchPin;
  this->AIN1 = AIN1;
  this->AIN2 = AIN2;
  this->PWMA = PWMA;
  this->PWMB = PWMB;
  this->BIN1 = BIN1;
  this->BIN2 = BIN2;

  distance = 0;
  backupTime = 300;
  turnTime = 200;
  switchedOn = false;
}

void AutonomousRobot::begin() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  
  Serial.begin(9600);
  Serial.print("To infinity and beyond!");
}

void AutonomousRobot::update() {
  distance = getDistance();

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" in");

  if (digitalRead(switchPin) == LOW) {
    if (distance < 10) {
      rightMotor(0);
      leftMotor(0);
      delay(200);
      rightMotor(-255);
      leftMotor(-255);
      delay(backupTime);
      rightMotor(255);
      leftMotor(-255);
      delay(turnTime);
    } else {
      rightMotor(255);
      leftMotor(255);
    }
    switchedOn = true;
  } else {
    rightMotor(0);
    leftMotor(0);
    switchedOn = false;
  }

  delay(50);
}

void AutonomousRobot::rightMotor(int motorSpeed) {
  if (motorSpeed > 0) {
    digitalWrite(AIN1, HIGH);
    digitalWrite(AIN2, LOW);
  } else if (motorSpeed < 0) {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, HIGH);
  } else {
    digitalWrite(AIN1, LOW);
    digitalWrite(AIN2, LOW);
  }
  analogWrite(PWMA, abs(motorSpeed));                 //now that the motor direction is set, drive it at the entered speed
}

void AutonomousRobot::leftMotor(int motorSpeed) {
  if (motorSpeed > 0) {
    digitalWrite(BIN1, HIGH);
    digitalWrite(BIN2, LOW);
  } else if (motorSpeed < 0) {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, HIGH);
  } else {
    digitalWrite(BIN1, LOW);
    digitalWrite(BIN2, LOW);
  }
    analogWrite(PWMB, abs(motorSpeed));                 //now that the motor direction is set, drive it at the entered speed
}

float AutonomousRobot::getDistance() {
  float echoTime;
  float calculatedDistance;

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  echoTime = pulseIn(echoPin, HIGH);
  calculatedDistance = echoTime / 148.0;

  return calculatedDistance;
}

bool AutonomousRobot::isSwitchedOn() {
  return switchedOn;
}
