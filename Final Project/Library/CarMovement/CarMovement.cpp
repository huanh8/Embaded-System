#include "CarMovement.h"

CarMovement::CarMovement(int ain1, int ain2, int pwma, int bin1, int bin2, int pwmb, int backupTime, int turnTime) {
  _ain1 = ain1;
  _ain2 = ain2;
  _pwma = pwma;
  _bin1 = bin1;
  _bin2 = bin2;
  _pwmb = pwmb;
  _backupTime = backupTime;
  _turnTime = turnTime;

  pinMode(_ain1, OUTPUT);
  pinMode(_ain2, OUTPUT);
  pinMode(_pwma, OUTPUT);
  pinMode(_bin1, OUTPUT);
  pinMode(_bin2, OUTPUT);
  pinMode(_pwmb, OUTPUT);
}

void CarMovement::forward() {
  rightMotor(255);
  leftMotor(255);
}

void CarMovement::backward() {
  rightMotor(-255);
  leftMotor(-255);
  delay(_backupTime);
  stop();
}

void CarMovement::turnLeft() {
  rightMotor(255);
  leftMotor(-255);
  delay(_turnTime);
  stop();
}

void CarMovement::turnRight() {
  rightMotor(-255);
  leftMotor(255);
  delay(_turnTime);
  stop();
}

void CarMovement::stop() {
  rightMotor(0);
  leftMotor(0);
}

void CarMovement::rightMotor(int speed) {
  if (speed > 0) {
    digitalWrite(_ain1, HIGH);
    digitalWrite(_ain2, LOW);
  } else if (speed < 0) {
    digitalWrite(_ain1, LOW);
    digitalWrite(_ain2, HIGH);
  } else {
    digitalWrite(_ain1, LOW);
    digitalWrite(_ain2, LOW);
  }

  analogWrite(_pwma, abs(speed));
}

void CarMovement::leftMotor(int speed) {
  if (speed > 0) {
    digitalWrite(_bin1, HIGH);
    digitalWrite(_bin2, LOW);
  } else if (speed < 0) {
    digitalWrite(_bin1, LOW);
    digitalWrite(_bin2, HIGH);
  } else {
    digitalWrite(_bin1, LOW);
    digitalWrite(_bin2, LOW);
  }

  analogWrite(_pwmb, abs(speed));
}
