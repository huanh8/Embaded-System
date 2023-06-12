#include <IRremote.h>
#include <CarMovement.h>

const int IR_PIN = 4;
const int SWITCH_PIN = 7;
const int AIN1 = 13;
const int AIN2 = 12;
const int PWMA = 11;
const int BIN1 = 8;
const int BIN2 = 9;
const int PWMB = 10;
const int BACKUP_TIME = 100;
const int TURN_TIME = 200;


// IR remote control codes
const unsigned long FORWARD_CODE = 3877175040;
const unsigned long BACKWARD_CODE = 2907897600;
const unsigned long LEFT_CODE = 4144561920;
const unsigned long RIGHT_CODE = 2774204160;
const unsigned long STOP_CODE = 3810328320;

IRrecv irReceiver(IR_PIN);
CarMovement car(AIN1, AIN2, PWMA, BIN1, BIN2, PWMB, BACKUP_TIME, TURN_TIME);

enum RobotState {
  IDLE,
  FORWARD,
  BACKWARD,
  TURN_LEFT,
  TURN_RIGHT
};

RobotState currentState = IDLE;

void setup() {
  pinMode(SWITCH_PIN, INPUT_PULLUP);
  irReceiver.enableIRIn();
  Serial.begin(9600);
  Serial.println("To infinity and beyond!");
}


void loop() {
  checkSwitch();
  checkSignal();
  executeState(currentState);
  delay(50);
}

void checkSwitch() {
  if (digitalRead(SWITCH_PIN) == LOW) {
    currentState = IDLE;
    Serial.println("Switch off");
    return;
  }
}

void checkSignal()
{
    if (irReceiver.decode()) {
    unsigned long code = irReceiver.decodedIRData.decodedRawData;
    Serial.println(code);
    switch (code) {
      case FORWARD_CODE:
        currentState = FORWARD;
        break;
      case BACKWARD_CODE:
        currentState = BACKWARD;
        break;
      case LEFT_CODE:
        currentState = TURN_LEFT;
        break;
      case RIGHT_CODE:
        currentState = TURN_RIGHT;
        break;
      case STOP_CODE:
        currentState = IDLE;
        break;
    }
    irReceiver.resume();
  }
}

void executeState(RobotState state) {
  switch (state) {
    case IDLE:
      car.stop();
      break;
    case FORWARD:
      car.forward();
      break;
    case BACKWARD:
      car.backward();
      break;
    case TURN_LEFT:
      car.turnLeft();
      break;
    case TURN_RIGHT:
      car.turnRight();
      break;
  }
}
