#include <IRremote.h>
#include <CarMovement.h>

// IR receiver pin
const int IR_PIN = 4;

// Switch pin
const int SWITCH_PIN = 7;

// Motor control pins
//the right motor will be controlled by the motor A pins on the motor driver
const int AIN1 = 13;
const int AIN2 = 12;
const int PWMA = 11;

//the left motor will be controlled by the motor B pins on the motor driver
const int BIN1 = 8;
const int BIN2 = 9;
const int PWMB = 10;
const int BACKUP_TIME = 100;
const int TURN_TIME = 200;

// Buzzer pin
const int BUZZER_PIN = 6;

// IR remote control codes
const unsigned long FORWARD_CODE = 3877175040;        // key 2
const unsigned long BACKWARD_CODE = 2907897600;       // key 8
const unsigned long LEFT_CODE = 4144561920;           // key 4
const unsigned long RIGHT_CODE = 2774204160;          // key 6
const unsigned long STOP_CODE = 3810328320;           // key 5
const unsigned long INTERACT_CODE = 4077715200;       // key 1

IRrecv irReceiver(IR_PIN);
CarMovement car(AIN1, AIN2, PWMA, BIN1, BIN2, PWMB, BACKUP_TIME, TURN_TIME);

enum RobotState {
  IDLE,
  FORWARD,
  BACKWARD,
  TURN_LEFT,
  TURN_RIGHT,
  INTERACT
};

RobotState currentState = IDLE;
//volatile boolean isInteract;
boolean isInteract;

void setup() {

  pinMode(SWITCH_PIN, INPUT_PULLUP);
  irReceiver.enableIRIn();
  pinMode(BUZZER_PIN, OUTPUT);    //set the output pin for the speaker
  Serial.begin(9600);
  Serial.println("To infinity and beyond!");
}

void loop() {
  checkSwitch();
  checkSignal();
  executeState(currentState);
  // interact();
  delay(50);
}

/********************************************************************************/

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
        Serial.println("FORWARD");
        break;
      case BACKWARD_CODE:
        currentState = BACKWARD;
        Serial.println("BACKWARD");
        break;
      case LEFT_CODE:
        currentState = TURN_LEFT;
        Serial.println("TURN_LEFT");
        break;
      case RIGHT_CODE:
        currentState = TURN_RIGHT;
        Serial.println("TURN_RIGHT");
        break;
      case STOP_CODE:
        currentState = IDLE;
        Serial.println("IDLE");
        break;
      case INTERACT_CODE:
        Serial.println("IDLE and INTERACT");
        currentState = INTERACT;
        // isInteract = true;
        break;
      default:
        // isInteract = false;
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
      interact();
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
      case INTERACT:
      interact();
      break;
  }
}

void interact()
{

    //horn();
    // if (isInteract)
    // {
    //   isInteract = false;
      // irReceiver.resume();
      tone(BUZZER_PIN, 440, 500); // Play the horn sound
      delay(150);
      Serial.println("interact.....");
      noTone(BUZZER_PIN);
      delay(150);

    // }
}

void horn()
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