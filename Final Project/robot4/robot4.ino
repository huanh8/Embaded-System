#include <IRremote.h>  // Include the IRremote library

#define ST_IDLE 1
#define ST_FORWARD 2
#define ST_BACKWARD 3
#define ST_TURN_LEFT 4
#define ST_TURN_RIGHT 5
#define ST_NONE 6

// IR remote control pin
const int IR_PIN = 4;

// IR remote control codes
const unsigned long FORWARD_CODE = 3877175040;
const unsigned long BACKWARD_CODE = 2907897600;
const unsigned long LEFT_CODE = 4144561920;
const unsigned long RIGHT_CODE = 2774204160;
const unsigned long STOP_CODE = 3810328320;

const int AIN1 = 13;           //control pin 1 on the motor driver for the right motor
const int AIN2 = 12;            //control pin 2 on the motor driver for the right motor
const int PWMA = 11;            //speed control pin on the motor driver for the right motor

//the left motor will be controlled by the motor B pins on the motor driver
const int PWMB = 10;           //speed control pin on the motor driver for the left motor
const int BIN2 = 9;           //control pin 2 on the motor driver for the left motor
const int BIN1 = 8;           //control pin 1 on the motor driver for the left motor

int switchPin = 7;             //switch to turn the robot on and off
// Distance variables
const int trigPin = 6;
const int echoPin = 5;

// Robot behavior variables
int backupTime = 100;
int turnTime = 200;

// Distance measurement variables
float distance = 0;

// IR receiver instance
IRrecv irReceiver(IR_PIN);

// Robot states
enum RobotState {
  IDLE,
  FORWARD,
  BACKWARD,
  TURN_LEFT,
  TURN_RIGHT
};

// RobotState currentState = IDLE;
int currentState = ST_NONE;
void setup()
{
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //set the motor control pins as outputs
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);

  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);

  pinMode(switchPin, INPUT_PULLUP);   //set this as a pullup to sense whether the switch is flipped

  irReceiver.enableIRIn();

  Serial.begin(9600);
  Serial.println("To infinity and beyond!");
}

void loop()
{
  if (digitalRead(switchPin) == LOW) 
  {  currentState = IDLE;
      Serial.println("switch off");
    return;
  }

  distance = getDistance();
  	// if (distance == 0 && currentState !=IDLE)
    // {
    //   return;
    // }

  // if (distance < 5) {
  //     Serial.print("Distance: ");
  //     Serial.print(distance);
  //     Serial.println(" in");
  //     stopRobot();
  // } else {
    if (irReceiver.decode()) {
      unsigned long code = irReceiver.decodedIRData.decodedRawData;
      Serial.println(code);
      switch (code) {
        case FORWARD_CODE:
          currentState = ST_FORWARD;
          break;
        case BACKWARD_CODE:
          currentState = ST_BACKWARD;
          break;
        case LEFT_CODE:
          currentState = ST_TURN_LEFT;
          break;
        case RIGHT_CODE:
          currentState = ST_TURN_RIGHT;
          break;
        case STOP_CODE:
          currentState = ST_IDLE;
          break;
        default:
        currentState = ST_NONE;
        break;
      }

      irReceiver.resume();
    }
  // }

  executeState(currentState);

  delay(50);
}

void executeState(RobotState state)
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
    default:
      // irReceiver.resume();
      break;
  }
}


void driveForward()
{
  Serial.println("Forward");
  leftMotor(200);
  rightMotor(200);
  // delay(backupTime);
  // stopRobot();
}

void driveBackward()
{
  Serial.println("Backward");
  rightMotor(-200);
  leftMotor(-200);
  // delay(backupTime);
  // stopRobot();
}

void turnLeft()
{
  Serial.println("turnLeft");
  leftMotor(-100);  
  rightMotor(100);    
  // delay(turnTime);
  // stopRobot();
}

void turnRight()
{
  Serial.println("turnRight");
  leftMotor(100); 
  rightMotor(-100);                           
  // delay(turnTime);
  // stopRobot();
}

void stopRobot()
{
  rightMotor(0);
  leftMotor(0);
  Serial.println("stopRobot");
}

/********************************************************************************/
void rightMotor(int motorSpeed)                       //function for driving the right motor
{
  if (motorSpeed > 0)                                 //if the motor should drive forward (positive speed)
  {
    Serial.println("rightMotor motorSpeed > 0");
    Serial.println(motorSpeed);
    digitalWrite(AIN1, HIGH);                         //set pin 1 to high
    digitalWrite(AIN2, LOW);                          //set pin 2 to low
  }
  else if (motorSpeed < 0)                            //if the motor should drive backward (negative speed)
  {
      Serial.println("rightMotor motorSpeed < 0");
      Serial.println(motorSpeed);
    digitalWrite(AIN1, LOW);                          //set pin 1 to low
    digitalWrite(AIN2, HIGH);                         //set pin 2 to high
  }
  else                                                //if the motor should stop
  {
    Serial.println("rightMotor motorSpeed LOW");
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
      Serial.println("leftMotor motorSpeed > 0");
    Serial.println(motorSpeed);
    digitalWrite(BIN1, HIGH);                         //set pin 1 to high
    digitalWrite(BIN2, LOW);                          //set pin 2 to low
  }
  else if (motorSpeed < 0)                            //if the motor should drive backward (negative speed)
  {
    Serial.println("leftMotor motorSpeed < 0");
     Serial.println(motorSpeed);
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


float getDistance()
{
  float echoTime;
  float calculatedDistance;

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  echoTime = pulseIn(echoPin, HIGH);

  calculatedDistance = echoTime / 148.0;

  return calculatedDistance;
}
