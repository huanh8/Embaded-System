#ifndef CarMovement_h
#define CarMovement_h

#include <Arduino.h>

class CarMovement {
  public:
    CarMovement(int ain1, int ain2, int pwma, int bin1, int bin2, int pwmb, int backupTime, int turnTime);
    void forward();
    void backward();
    void turnLeft();
    void turnRight();
    void stop();
  
  private:
    int _ain1;
    int _ain2;
    int _pwma;
    int _bin1;
    int _bin2;
    int _pwmb;
    int _backupTime;
    int _turnTime;

    void rightMotor(int speed);
    void leftMotor(int speed);
};

#endif
