#include "AFMotor.h"

#define CHAR_TABLE_LEGTH 39

const int LIGHT_PIN = 10;
const int LIMIT_LR = A0;
const int LIMIT_UD = A1;

const int SPEED = 100;
const int MIN_X = 245;
const int MIN_Y = 0;
const int MAX_X = 1600;
const int MAX_Y = 700;

//planchet's current position
int posX = 0;
int posY = 0;

long timeNextOn = 0;
long timeNextOff = 0;

AF_Stepper motorLR(200, 1);
AF_Stepper motorUD(200, 2);

int charTable[CHAR_TABLE_LEGTH][3] = {
  -1, -1, 'a',
  -1, -1, 'b',
  -1, -1, 'c',
  -1, -1, 'd',
  -1, -1, 'e',
  -1, -1, 'f',
  -1, -1, 'g',
  -1, -1, 'h',
  -1, -1, 'i',
  -1, -1, 'j',
  -1, -1, 'k',
  -1, -1, 'l',
  -1, -1, 'm',
  -1, -1, 'n',
  -1, -1, 'o',
  -1, -1, 'p',
  -1, -1, 'q',
  -1, -1, 'r',
  -1, -1, 's',
  -1, -1, 't',
  -1, -1, 'u',
  -1, -1, 'v',
  -1, -1, 'w',
  -1, -1, 'x',
  -1, -1, 'y',
  -1, -1, 'z',
  -1, -1, '0',
  -1, -1, '1',
  -1, -1, '2',
  -1, -1, '3',
  -1, -1, '4',
  -1, -1, '5',
  -1, -1, '6',
  -1, -1, '8',
  -1, -1, '9',
  -1, -1, '+', //YES
  -1, -1, '-', //NO
  -1, -1, '!', //GOODBYE
  -1, -1, '.' //SPACE
};

void setup() {
  Serial.begin(9600);
  Serial.println("[INFO] Initalising...");

  pinMode(LIGHT_PIN, OUTPUT);
  digitalWrite(LIGHT_PIN, HIGH);

  pinMode(LIMIT_LR, INPUT_PULLUP);
  pinMode(LIMIT_UD, INPUT_PULLUP);

  motorLR.setSpeed(SPEED);
  motorUD.setSpeed(SPEED);

  Serial.println("[INFO] Resetting...");
  delay(100);
  reset();
  delay(100);
  Serial.println("[INFO] Welcome!");
  delay(100);
  
}

void loop() {
  //serialCalbrate();
}

void reset() {
  //Move until we hit the limit switches
  while (digitalRead(LIMIT_LR) != LOW) {
    motorLR.step(1, FORWARD, DOUBLE);
  }
  motorLR.release();

  while (digitalRead(LIMIT_UD) != LOW) {
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

int findSymbolInTable(char symbol)
{
  for (int tableIndex = 0; tableIndex <= CHAR_TABLE_LEGTH; tableIndex++) {
    if (symbol == char(charTable[tableIndex][2])) {
      return tableIndex;
    }
  }
  return -2;
}

void moveToSymbol(char character)
{
  int tableIndex = findSymbolInTable(character);
  int toX = charTable[tableIndex][0];
  int toY = charTable[tableIndex][1];

  if (toX == -1 || toY == -1) {
    //Serial.println(String("[ERROR] Character ") + String(character) + String(" does not have a valid XY coords! (X: ") + String(toX) + String(" Y: ") + String(toY));
  }

  if (toX == -2 || toY == -2) {
    //Serial.println(String("[ERROR] Character ") + String(character) + String(" does not exist in the character list!"));
  }
  move(toX, toY);
}


void move(int tx, int ty) { //Async sometime

  int dirX, dirY;
  int moveX, moveY;

  if (tx < MIN_X) {
    tx = MIN_X;
  }
  if (ty < MIN_Y) {
    ty = MIN_Y;
  }

  if (tx > MAX_X) {
    tx = MAX_X;
  }
  if (ty > MAX_Y) {
    ty = MAX_Y;
  }

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

  motorUD.step(moveY, dirY, DOUBLE);

  posX = tx;
  posY = ty;
}

void release()
{
  motorLR.release();
  motorUD.release();
}

//===================== Test Code ====================

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
      move(posX, posY + 5);
    }
    else if (in == 'W') {
      move(posX, posY + 50);
    }

    else if (in == 's') {
      move(posX, posY - 5);
    }
    else if (in == 'S') {
      move(posX, posY - 50);
    }

    else if (in == 'a') {
      move(posX - 5, posY);
    }
    else if (in == 'A') {
      move(posX - 50, posY);
    }

    else if (in == 'd') {
      move(posX + 5, posY);
    }
    else if (in == 'D') {
      move(posX + 50, posY);
    }

    release();

    Serial.print("[DEBUG] X: ");
    Serial.print(posX, DEC);
    Serial.print(" Y: ");
    Serial.print(posY, DEC);
    Serial.println("");
  }
}

// https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
void line(int x0, int y0, int x1, int y1) {
 
  int dx = abs(x1-x0), sx = x0<x1 ? 1 : -1;
  int dy = abs(y1-y0), sy = y0<y1 ? 1 : -1; 
  int err = (dx>dy ? dx : -dy)/2, e2;
 
  while(true) {
    //move(x0,y0);
    Serial.print("M("); Serial.print(x0); Serial.print(","); Serial.print(y0); Serial.println(")");
    if (x0==x1 && y0==y1) 
      break;
    e2 = err;
    if (e2 >-dx) { err -= dy; x0 += sx; }
    if (e2 < dy) { err += dx; y0 += sy; }
  }
}



