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

// Buzzer pin
const int BUZZER_PIN = 6;

// State definitions
#define ST_IDLE 1
#define ST_FORWARD 2
#define ST_BACKWARD 3
#define ST_TURN_LEFT 4
#define ST_TURN_RIGHT 5 
// #define ST_HORN 6 

// IR Remote codes
#define FORWARD_CODE 3877175040     // key 2
#define BACKWARD_CODE 2907897600    // key 8
#define LEFT_CODE 4144561920        // key 4
#define RIGHT_CODE 2774204160       // key 6
#define STOP_CODE 3810328320        // key 5
// #define BEEP_CODE 2707357440        // key 3


IRrecv irReceiver(IR_PIN);

int currentState = ST_IDLE;

/********************************************************************************/
void setup()
{
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);    //set the output pin for the speaker
  carHorn();
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);
  irReceiver.enableIRIn();

  Serial.begin(9600);
  Serial.println("Enter a key");

}

/********************************************************************************/
void loop()
{
  if (digitalRead(SWITCH_PIN) == HIGH) {
    Serial.println("switch off");
    return;
  }
  if (irReceiver.decode()) {
    unsigned long irCode = irReceiver.decodedIRData.decodedRawData;
    Serial.println(irCode);
    if (irCode == FORWARD_CODE) {
      changeState(ST_FORWARD);
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
  delay(50);
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


/********************************************************************************/
void carHorn()
{
  int hornFrequency = 440; // Set the initial frequency of the horn sound
  int hornDuration = 500;  // Set the initial duration of the horn sound
  int hornPause = 100;     // Set the pause between horn sounds

  for (int i = 0; i < 3; i++) { // Play the horn sound three times
    tone(BUZZER_PIN, hornFrequency, hornDuration); // Play the horn sound
    delay(hornDuration + hornPause); // Wait for the sound to finish and add a pause
  }
  noTone(BUZZER_PIN); // Stop the buzzer
}