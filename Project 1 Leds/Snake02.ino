/*************************************************** 
  This is a library for our I2C LED Backpacks

  Designed specifically to work with the Adafruit LED Matrix backpacks 
  ----> http://www.adafruit.com/products/872
  ----> http://www.adafruit.com/products/871
  ----> http://www.adafruit.com/products/870

  These displays use I2C to communicate, 2 pins are required to 
  interface. There are multiple selectable I2C addresses. For backpacks
  with 2 Address Select pins: 0x70, 0x71, 0x72 or 0x73. For backpacks
  with 3 Address Select pins: 0x70 thru 0x77

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
// #include <avr/wdt.h> // Include the Watchdog Timer library
// Snake on 8x8Matrix 
// 2013-06-15 JorgVisch
// Reference :Jvisch. (2013, August 23). Snake 8x8 led matrix (ada fruit). Fritzing. https://fritzing.org/projects/snake-8x8-led-matrix-ada-fruit


static const uint8_t PROGMEM
  smile_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10100101,
    B10011001,
    B01000010,
    B00111100 },
  neutral_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10111101,
    B10000001,
    B01000010,
    B00111100 },
  frown_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10011001,
    B10100101,
    B01000010,
    B00111100 };

// joystick 
const int varXPin = A1;//X Value  from Joystick
const int varYPin = A2;//Y Value from Joystick

// sound
const int buzzerPin = 10;

//restart button
const int restartButtonPin = 2;

//Leds and tones
int led[] = {3, 5, 7, 9};     //red is led[0], yellow is led[1], green is led[2], blue is led[3]
int tones[] = {262, 330, 392, 494};   //tones to play with each button (c, e, g, b)


// direction
const int TOP    = 0;
const int RIGHT  = 1;
const int BOTTOM = 2;
const int LEFT   = 3;
// Snake
const int MAX_SNAKE_LENGTH = 10;

// Variables
// Adafruit_8x8matrix matrix = Adafruit_8x8matrix();  // Display
int direction = TOP;                               // direction of movement
int snakeX[MAX_SNAKE_LENGTH];                      // X-coordinates of snake
int snakeY[MAX_SNAKE_LENGTH];                      // Y-coordinates of snake
int snakeLength = 1;                               // nr of parts of snake
boolean buttonRead = false;                        // is button already read in this loop
unsigned long prevTime = 0;                        // for gamedelay (ms)
unsigned long delayTime = 500;                     // Game step in ms

int fruitX, fruitY;
unsigned long fruitPrevTime = 0;
unsigned long fruitBlinkTime = 1000/5;
int fruitLed = LED_YELLOW;
boolean isWon;
boolean isBorderHit; 
volatile boolean isRestart;

Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

void setup() {

  attachInterrupt(digitalPinToInterrupt(restartButtonPin), buttonPressed, CHANGE);



  
  Serial.begin(9600);
  Serial.println("Snake is started");
  randomSeed(analogRead(0));
  pinMode(restartButtonPin, INPUT_PULLUP);

  //set the buzzer pin as an output
  pinMode(buzzerPin, OUTPUT);

  //Set Joystick Pins as INPUTs
  pinMode(varXPin, INPUT);
  pinMode(varYPin, INPUT);
  isWon = false;
  isBorderHit = false;

  // Init led matrix
  matrix.begin(0x70);

  // init snake
  snakeX[0] = 4;
  snakeY[0] = 7;
  for(int i=1; i<MAX_SNAKE_LENGTH; i++){
    snakeX[i] = snakeY[i] = -1;
  }
  makeFruit();
}

void loop(){
  checkButtons();
  unsigned long currentTime = millis();
  if(currentTime - prevTime >= delayTime){
    nextStep(); 
    buttonRead = false;
    prevTime = currentTime;
  }   
  //  if(digitalRead(restartButtonPin) == LOW){
  //   restart();
  //   return;
  // }

  checkButton();
  if(!isRestart){
    checkGameOver();
  }

}
void buttonPressed()
{
  if(digitalRead(restartButtonPin) == HIGH)
  {
    isRestart= true;
  }
}
void checkButton()
{
    if (isRestart)
  {
    restart();
    isRestart = false;
  }
  else
  {
    isRestart = false;
  }
}
void checkGameOver(){
  if(isWon){
    drawSmile();
    song();
    isWon = false;
    return;
  }
  else if(isBorderHit){
    drawFrown();
    loseSequence();
    isBorderHit = false;
    return;
  }
  else{
    draw();
  }
}

void restart(){
  winSequence();
  matrix.clear();
  delay(100);
  setup();
}

void checkButtons(){
  if(!buttonRead){
    int currentDirection = direction;
    int joyX = analogRead(varXPin);
    int joyY = analogRead(varYPin);
    if(joyY < 100){
      direction = TOP;
    }
    else if(joyY > 920){
      direction = BOTTOM;
    }
    else if(joyX > 920){
      direction = RIGHT;
    }
    else if(joyX < 100){
      direction = LEFT;
    }
    buttonRead = (currentDirection != direction);
  }
}
void draw(){
  matrix.clear();
  drawSnake();
  drawFruit();
  matrix.writeDisplay();
}

void drawSnake(){
  for(int i=0; i<snakeLength; i++){
    matrix.drawPixel(snakeX[i], snakeY[i], LED_ON);
  }
}

void drawFruit(){
  if(inPlayField(fruitX, fruitY)){
    unsigned long currenttime = millis();
    if(currenttime - fruitPrevTime >= fruitBlinkTime){
      fruitLed = (fruitLed == LED_YELLOW) ? LED_OFF : LED_YELLOW;
      fruitPrevTime = currenttime;
    }
    matrix.drawPixel(fruitX, fruitY, fruitLed);
  }
}

boolean inPlayField(int x, int y){
  return (x>=0) && (x<8) && (y>=0) && (y<8);
}

void nextStep(){

  for(int i=snakeLength-1; i>0; i--){
    snakeX[i] = snakeX[i-1];
    snakeY[i] = snakeY[i-1];
  }
  switch(direction){
    case TOP:
      snakeY[0] = snakeY[0]-1;
      break;
    case RIGHT:
      snakeX[0] = snakeX[0]+1;
      break;
    case BOTTOM:
      snakeY[0] = snakeY[0]+1;
      break;
    case LEFT:
      snakeX[0]=snakeX[0]-1;
      break;
  }
  
  if((snakeX[0] == fruitX) && (snakeY[0] == fruitY)){
    snakeLength++;
    if(snakeLength < MAX_SNAKE_LENGTH){      
      makeFruit();
    } 
    else {
      fruitX = fruitY = -1;
      isWon = true;
    }
  }

  // check if the snake collides with itself
  for(int i=1; i<snakeLength; i++){
    if(snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]){
      isBorderHit = true;
      return;
    }
  }

  // check if snake hits the border
  if(snakeX[0] < 0 || snakeX[0] >= matrix.width() || snakeY[0] < 0 || snakeY[0] >= matrix.height()){
    isBorderHit = true;
    return;
  }
}

void makeFruit(){
  int x, y;
  x = random(0, 8);
  y = random(0, 8);
  while(isPartOfSnake(x, y)){
    x = random(0, 8);
    y = random(0, 8);
  }
  fruitX = x;
  fruitY = y;
}

boolean isPartOfSnake(int x, int y){
  for(int i=0; i<snakeLength-1; i++){
    if((x == snakeX[i]) && (y == snakeY[i])){
      return true;
    }
  }
  return false;
}

//draw faces
void drawSmile(){
  matrix.clear();
  matrix.drawBitmap(0, 0, smile_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, neutral_bmp, 8, 8, LED_GREEN);
  matrix.writeDisplay();
  delay(500);
}
void drawFrown(){
  matrix.clear();
  matrix.drawBitmap(0, 0, frown_bmp, 8, 8, LED_RED);
  matrix.writeDisplay();
  delay(500);
  matrix.clear();
  matrix.drawBitmap(0, 0, neutral_bmp, 8, 8, LED_RED);
  matrix.writeDisplay();
  delay(500);
}



/*
  SparkFun Inventor’s Kit
  Circuit 2C-Simon Says

  The Simon Says game flashes a pattern using LED lights, then the player must repeat the pattern.

  This sketch was written by SparkFun Electronics, with lots of help from the Arduino community.
  This code is completely free for any use.

  View circuit diagram and instructions at: https://learn.sparkfun.com/tutorials/sparkfun-inventors-kit-experiment-guide---v41
  Download drawings and code at: https://github.com/sparkfun/SIK-Guide-Code
*/

//----------FUNCTIONS------------

//FLASH LED
void flashLED (int ledNumber) {
  digitalWrite(led[ledNumber], HIGH);
  tone(buzzerPin, tones[ledNumber]);
}

//TURN ALL LEDS OFF
void allLEDoff () {
  //turn all the LEDs off
  digitalWrite(led[0], LOW);
  digitalWrite(led[1], LOW);
  digitalWrite(led[2], LOW);
  digitalWrite(led[3], LOW);
  //turn the buzzer off
  noTone(buzzerPin);
}
//WIN SEQUENCE
void winSequence() {

  //turn all the LEDs on
  for (int j = 0; j <= 3; j++) {
    digitalWrite(led[j], HIGH);
  }

  //play the 1Up noise
  tone(buzzerPin, 1318, 150);   //E6
  delay(175);
  tone(buzzerPin, 1567, 150);   //G6
  delay(175);
  tone(buzzerPin, 2637, 150);   //E7
  delay(175);
  tone(buzzerPin, 2093, 150);   //C7
  delay(175);
  tone(buzzerPin, 2349, 150);   //D7
  delay(175);
  tone(buzzerPin, 3135, 500);   //G7
  delay(500);

  //wait until a button is pressed
  // do {
  //   pressedButton = buttonCheck();
  // } while (pressedButton > 3);
  delay(100);
  allLEDoff();

  // gameStarted = false;   //reset the game so that the start sequence will play again.

}

//LOSE SEQUENCE
void loseSequence() {

  //turn all the LEDs on
  for (int j = 0; j <= 3; j++) {
    digitalWrite(led[j], HIGH);
  }

  //play the 1Up noise
  tone(buzzerPin, 130, 250);   //E6
  delay(275);
  tone(buzzerPin, 73, 250);   //G6
  delay(275);
  tone(buzzerPin, 65, 150);   //E7
  delay(175);
  tone(buzzerPin, 98, 500);   //C7
  delay(500);

  //wait until a button is pressed
  // do {
  //   pressedButton = buttonCheck();
  // } while (pressedButton > 3);
  allLEDoff();
  delay(200);
}

/*
  SparkFun Inventor’s Kit
  Circuit 2A - Buzzer

  Play notes using a buzzer connected to pin 10

  This sketch was written by SparkFun Electronics, with lots of help from the Arduino community.
  This code is completely free for any use.

  View circuit diagram and instructions at: https://learn.sparkfun.com/tutorials/sparkfun-inventors-kit-experiment-guide---v41
  Download drawings and code at: https://github.com/sparkfun/SIK-Guide-Code
*/
void song()
{ play('g', 2);       //ha
  play('g', 1);       //ppy
  play('a', 4);       //birth
  play('g', 4);       //day
  play('C', 4);       //to
  play('b', 4);       //you

  play(' ', 2);       //pause for 2 beats

  play('g', 2);       //ha
  play('g', 1);       //ppy
  play('a', 4);       //birth
  play('g', 4);       //day
  play('D', 4);       //to
  play('C', 4);       //you

  play(' ', 2);       //pause for 2 beats

  play('g', 2);       //ha
  play('g', 1);       //ppy
  play('G', 4);       //birth
  play('E', 4);       //day
  play('C', 4);       //dear
  play('b', 4);       //your
  play('a', 6);       //name

  play(' ', 2);       //pause for 2 beats

  play('F', 2);       //ha
  play('F', 1);       //ppy
  play('E', 4);       //birth
  play('C', 4);       //day
  play('D', 4);       //to
  play('C', 6);       //you

  while (true) {}     //get stuck in this loop forever so that the song only plays once}
}
void play( char note, int beats)
{
  int numNotes = 14;  // number of notes in our note and frequency array (there are 15 values, but arrays start at 0)

  //Note: these notes are C major (there are no sharps or flats)

  //this array is used to look up the notes
  char notes[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C', 'D', 'E', 'F', 'G', 'A', 'B', ' '};
  //this array matches frequencies with each letter (e.g. the 4th note is 'f', the 4th frequency is 175)
  int frequencies[] = {131, 147, 165, 175, 196, 220, 247, 262, 294, 330, 349, 392, 440, 494, 0};

  int currentFrequency = 0;    //the frequency that we find when we look up a frequency in the arrays
  int beatLength = 150;   //the length of one beat (changing this will speed up or slow down the tempo of the song)

  //look up the frequency that corresponds to the note
  for (int i = 0; i < numNotes; i++)  // check each value in notes from 0 to 14
  {
    if (notes[i] == note)             // does the letter passed to the play function match the letter in the array?
    {
      currentFrequency = frequencies[i];   // Yes! Set the current frequency to match that note
    }
  }

  //play the frequency that matched our letter for the number of beats passed to the play function
  tone(buzzerPin, currentFrequency, beats * beatLength);
  delay(beats * beatLength);  //wait for the length of the tone so that it has time to play
  delay(50);                  //a little delay between the notes makes the song sound more natural

}
