#include <IRremote.h>  // Include the IRremote library

// IR receiver pin
const int IR_PIN = 4;

// Motor control pins
//the right motor will be controlled by the motor A pins on the motor driver
const int AIN1 = 13;
const int AIN2 = 12;
const int PWMA = 11;

//the left motor will be controlled by the motor B pins on the motor driver
const int PWMB = 10;           //speed control pin on the motor driver for the left motor
const int BIN2 = 9;           //control pin 2 on the motor driver for the left motor
const int BIN1 = 8;           //control pin 1 on the motor driver for the left motor

// Switch pin
const int SWITCH_PIN = 7;

// Motor speed
const int MOTOR_SPEED = 200;

// State definitions
#define ST_IDLE 1
#define ST_FORWARD 2
#define ST_BACKWARD 3
#define ST_TURN_LEFT 4
#define ST_TURN_RIGHT 5

// IR Remote codes
#define FORWARD_CODE 3877175040
#define BACKWARD_CODE 2907897600
#define LEFT_CODE 4144561920
#define RIGHT_CODE 2774204160
#define STOP_CODE 3810328320


IRrecv irReceiver(IR_PIN);

int currentState = ST_IDLE;

/********************************************************************************/
void setup()
{
  pinMode(SWITCH_PIN, INPUT_PULLUP);

  // pinMode(BIN1, OUTPUT);
  // pinMode(BIN2, OUTPUT);
  // pinMode(PWMB, OUTPUT);
  
  // pinMode(AIN1, OUTPUT);
  // pinMode(AIN2, OUTPUT);
  // pinMode(PWMA, OUTPUT);
  irReceiver.enableIRIn();

  Serial.begin(9600);
  Serial.println("Enter a direction followed by a distance.");
}

/********************************************************************************/
void loop()
{

  if (irReceiver.decode()) {
    unsigned long irCode = irReceiver.decodedIRData.decodedRawData;
    Serial.println(irCode);
    if (irCode == FORWARD_CODE) {
      // changeState(ST_FORWARD);
        rightMotor(200);                                //drive the right wheel forward   problem here....................
        leftMotor(200);                                 //drive the left wheel forward
        delay(1000);   

    } else if (irCode == BACKWARD_CODE) {
      changeState(ST_BACKWARD);
    } else if (irCode == LEFT_CODE) {
      changeState(ST_TURN_LEFT);
    } else if (irCode == RIGHT_CODE) {
      changeState(ST_TURN_RIGHT);
    }else if (irCode == STOP_CODE) {
      changeState(ST_IDLE);
    }
    irReceiver.resume();
  }

  executeState(currentState);
}

/********************************************************************************/
void changeState(int newState)
{
  currentState = newState;
}

/********************************************************************************/
void executeState(int state)
{
  switch (state) {
    case ST_IDLE:
      stopRobot();
      break;
    case ST_FORWARD:
      driveForward();
      break;
    case ST_BACKWARD:
      driveBackward();
      break;
    case ST_TURN_LEFT:
      turnLeft();
      break;
    case ST_TURN_RIGHT:
      turnRight();
      break;
  }
}

/********************************************************************************/
void stopRobot()
{
  rightMotor(0);
  leftMotor(0);
  currentState = ST_IDLE;
}

/********************************************************************************/
void driveForward()
{
  rightMotor(MOTOR_SPEED);
  leftMotor(MOTOR_SPEED);
}

/********************************************************************************/
void driveBackward()
{
  rightMotor(-MOTOR_SPEED);
  leftMotor(-MOTOR_SPEED);
}

/********************************************************************************/
void turnLeft()
{
  rightMotor(MOTOR_SPEED);
  leftMotor(-MOTOR_SPEED);
}

/********************************************************************************/
void turnRight()
{
  rightMotor(-MOTOR_SPEED);
  leftMotor(MOTOR_SPEED);
}

/********************************************************************************/
void rightMotor(int motorSpeed)                       //function for driving the right motor
{
  if (motorSpeed > 0)                                 //if the motor should drive forward (positive speed)
  {
    Serial.println(motorSpeed);
    digitalWrite(AIN1, HIGH);                         //set pin 1 to high
    digitalWrite(AIN2, LOW);                          //set pin 2 to low
  }
  else if (motorSpeed < 0)                            //if the motor should drive backward (negative speed)
  {
    digitalWrite(AIN1, LOW);                          //set pin 1 to low
    digitalWrite(AIN2, HIGH);                         //set pin 2 to high
  }
  else                                                //if the motor should stop
  {
    digitalWrite(AIN1, LOW);                          //set pin 1 to low
    digitalWrite(AIN2, LOW);                          //set pin 2 to low
  }
  analogWrite(PWMA, abs(motorSpeed));                 //now that the motor direction is set, drive it at the entered speed
}

/********************************************************************************/
void leftMotor(int motorSpeed)                        //function for driving the left motor
{
  if (motorSpeed > 0)                                 //if the motor should drive forward (positive speed)
  {
    digitalWrite(BIN1, HIGH);                         //set pin 1 to high
    digitalWrite(BIN2, LOW);                          //set pin 2 to low
  }
  else if (motorSpeed < 0)                            //if the motor should drive backward (negative speed)
  {
    digitalWrite(BIN1, LOW);                          //set pin 1 to low
    digitalWrite(BIN2, HIGH);                         //set pin 2 to high
  }
  else                                                //if the motor should stop
  {
    digitalWrite(BIN1, LOW);                          //set pin 1 to low
    digitalWrite(BIN2, LOW);                          //set pin 2 to low
  }
  analogWrite(PWMB, abs(motorSpeed));                 //now that the motor direction is set, drive it at the entered speed
}
