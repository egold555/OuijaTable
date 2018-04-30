#include "AFMotor.h"

#define CHAR_TABLE_LEGTH 39

const int LIGHT_PIN = 10;
const int LIMIT_LR = A0;
const int LIMIT_UD = A1;

const int SPEED = 100;
const int MIN_X = 100;
const int MIN_Y = 0;
const int MAX_X = 1500;
const int MAX_Y = 550;

//planchet's current position
int posX = 0;
int posY = 0;

long timeNextOn = 0;
long timeNextOff = 0;

AF_Stepper motorLR(200, 1);
AF_Stepper motorUD(200, 2);

unsigned int charTable[CHAR_TABLE_LEGTH][3] = {
  0, 0, 'a',
  0, 0, 'b',
  0, 0, 'c',
  0, 0, 'd',
  0, 0, 'e',
  0, 0, 'f',
  0, 0, 'g',
  0, 0, 'h',
  0, 0, 'i',
  0, 0, 'j',
  0, 0, 'k',
  0, 0, 'l',
  0, 0, 'm',
  0, 0, 'n',
  0, 0, 'o',
  0, 0, 'p',
  0, 0, 'q',
  0, 0, 'r',
  0, 0, 's',
  0, 0, 't',
  0, 0, 'u',
  0, 0, 'v',
  0, 0, 'w',
  0, 0, 'x',
  0, 0, 'y',
  0, 0, 'z',
  0, 0, '0',
  0, 0, '1',
  0, 0, '2',
  0, 0, '3',
  0, 0, '4',
  0, 0, '5',
  0, 0, '6',
  0, 0, '8',
  0, 0, '9',
  0, 0, '+', //YES
  0, 0, '-', //NO
  0, 0, '!', //GOODBYE
  0, 0, '.' //SPACE
};

void setup() {
  Serial.begin(9600);
  Serial.println("Initalising...");
  
  pinMode(LIGHT_PIN, OUTPUT);
  digitalWrite(LIGHT_PIN, HIGH);

  pinMode(LIMIT_LR, INPUT_PULLUP);
  pinMode(LIMIT_UD, INPUT_PULLUP);
  
  motorLR.setSpeed(SPEED);
  motorUD.setSpeed(SPEED);

  Serial.println("Resetting...");
  delay(100);
  reset();
  delay(100);
  Serial.println("Welcome!");
}

void loop() {
  serialCalbrate();
}

void reset(){
  //Move until we hit the limit switches
  while(digitalRead(LIMIT_LR) != LOW){
    motorLR.step(1, FORWARD, DOUBLE);
  }
  motorLR.release();

  while(digitalRead(LIMIT_UD) != LOW){
    motorUD.step(1, BACKWARD, DOUBLE);
  }
  motorUD.release();

  posX = MAX_X;
  posY = 0;
  
}

void lightFlicker() {
  long currentTime = millis();
  bool on = !digitalRead(LIGHT_PIN);
  if (!on && currentTime > timeNextOn) {
    digitalWrite(LIGHT_PIN, LOW);
    timeNextOff = currentTime + random(100, 10000); //on for
  }
  else if (on && currentTime > timeNextOff) {
    digitalWrite(LIGHT_PIN, HIGH);
    timeNextOn = currentTime + random(40, 500); //off for
  }
}

void seekTest() {
  motorLR.step(MAX_X, FORWARD, DOUBLE);
  motorLR.release();
  motorUD.step(MAX_Y, FORWARD, DOUBLE);
  motorUD.release();
  delay(50);
  motorLR.step(MAX_X, BACKWARD, DOUBLE);
  motorLR.release();
  motorUD.step(MAX_Y, BACKWARD, DOUBLE);
  motorUD.release();
  delay(50);
}

const int MOVE_AMOUNT = 20;
void serialCalbrate() {
  if (Serial.available() > 0) {
    char in = (char)Serial.read();

    if (in == 'w') {
      move(posX, posY + 20);
    }
    else if (in == 'W') {
      move(posX, posY + 50);
    }
    
    else if (in == 's') {
      move(posX, posY - 20);
    }
    else if (in == 'S') {
      move(posX, posY - 50);
    }
    
    else if (in == 'a') {
      move(posX - 20, posY);
    }
    else if (in == 'A') {
      move(posX - 50, posY);
    }
    
    else if (in == 'd') {
      move(posX + 20, posY);
    }
    else if (in == 'D') {
      move(posX + 50, posY);
    }


    Serial.print("X: ");
    Serial.print(posX, DEC);
    Serial.print(" Y: ");
    Serial.print(posY, DEC);
    Serial.println("");
  }
}

int findSymbolInTable(char symbol)
{
  for (int tableIndex = 0; tableIndex <= CHAR_TABLE_LEGTH; tableIndex++) {
    if (symbol == char(charTable[tableIndex][2])) {
      return tableIndex;
    }
  }
  return -1;
}

void moveToSymbol(char character)
{
  int tableIndex = findSymbolInTable(character);
  int toX = charTable[tableIndex][0];
  int toY = charTable[tableIndex][1];
}

//void movePlus(int tx, int ty){
//  move(tx + posX, ty + posY);
//}

void move(int tx, int ty) { //Async sometime

  int dirX, dirY;
  int moveX, moveY;

  if(tx < MIN_X){tx = MIN_X;}
  if(ty < MIN_Y){ty = MIN_Y;}

  if(tx > MAX_X){tx = MAX_X;}
  if(ty > MAX_Y){ty = MAX_Y;}

  if (tx < posX) {
    dirX = BACKWARD;
    moveX = posX - tx;
  }
  else {
    dirX = FORWARD;
    moveX = tx - posX;
  }

  if (ty < posY) {
    dirY = BACKWARD;
    moveY = posY - ty;
  }
  else {
    dirY = FORWARD;
    moveY = ty - posY;
  }

  motorLR.step(moveX, dirX, DOUBLE);
  motorLR.release();
  
  motorUD.step(moveY, dirY, DOUBLE);
  motorUD.release();
  
  posX = tx;
  posY = ty;
}

