#ifndef AutonomousRobot_h
#define AutonomousRobot_h

#include "Arduino.h"

class AutonomousRobot {
  public:
    AutonomousRobot(int trigPin, int echoPin, int switchPin, int AIN1, int AIN2, int PWMA, int PWMB, int BIN1, int BIN2);
    void begin();
    void update();
    void rightMotor(int motorSpeed);
    void leftMotor(int motorSpeed);
    float getDistance();
    bool isSwitchedOn();

  private:
    int trigPin;
    int echoPin;
    int switchPin;
    int AIN1;
    int AIN2;
    int PWMA;
    int PWMB;
    int BIN1;
    int BIN2;
    float distance;
    int backupTime;
    int turnTime;
    bool switchedOn;
};

#endif
