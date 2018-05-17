#include "AFMotor.h"

const int LIGHT_PIN = 10;
const int LIMIT_LR = A0;
const int LIMIT_UD = A1;

const int SPEED = 100;
const int MIN_X = 400; //245
const int MIN_Y = 0;
const int MAX_X = 1800; //1600
const int MAX_Y = 700; //700

//planchet's current position
int posX = 0;
int posY = 0;

long timeNextOn = 0;
long timeNextOff = 0;

AF_Stepper motorLR(200, 1);
AF_Stepper motorUD(200, 2);

int charTable[][3] = {
  //-1, -1, 'a',
  700, 300, 'b',
  800, 300, 'c',
  890, 350, 'd',
  1050, 400, 'e',
  1190, 400, 'f',
  1300, 400, 'g',
  1480, 400, 'h',
  1540, 350, 'i',
  1620, 300, 'j',
  1800, 250, 'k',
  //-1, -1, 'l',
  //-1, -1, 'm',
  //-1, -1, 'n',
  //-1, -1, 'o',
  750, 80, 'p',
  910, 120, 'q',
  960, 190, 'r',
  1110, 200, 's',
  1260, 200, 't',
  1370, 180, 'u',
  1500, 180, 'v',
  1700, 120, 'w',
  1800, 20, 'x',
  //-1, -1, 'y',
  //-1, -1, 'z',
  //  -1, -1, '0',
  //  -1, -1, '1',
  //  -1, -1, '2',
  //  -1, -1, '3',
  //  -1, -1, '4',
  //  -1, -1, '5',
  //  -1, -1, '6',
  //  -1, -1, '8',
  //  -1, -1, '9',
  750, 550, '+', //YES
  1800, 550, '-', //NO
  //  -1, -1, '!', //GOODBYE
  1350, 550, '.' //SPACE
};

#define CHAR_TABLE_LEGTH (sizeof(charTable)/sizeof(charTable[0]))

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
  reset();
  Serial.println("[INFO] Successfully Initalised!");
  Serial.println("[INFO] Waiting 5 seconds for y calibration");
  delay(5000);

}

//called everytime a message finishes
void loop() {
  update();
  char *arrayThing = "testing";
  spell(arrayThing);
  //serialCalbrate();
  //letterTest();
}

//Called by motor and by loop!
void update() {
  lightFlicker();
  //Serial.println("Update");
}

//Gets called from modified stepper lib
void AF_Stepper::doupdate(){ 
  update();
}

void reset() {
  //Move until we hit the limit switches
  while (digitalRead(LIMIT_LR) != LOW) {
    motorLR.step(1, FORWARD, DOUBLE);
  }
  motorLR.release();

  //Until I fix swich
  //  while (digitalRead(LIMIT_UD) != LOW) {
  //    motorUD.step(1, BACKWARD, DOUBLE);
  //  }
  //  motorUD.release();

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
  return -1;
}

void moveToSymbol(char character)
{
  boolean can = true;
  if (character <= 31) {
    Serial.print("[ERROR] Character "); Serial.print(character, DEC); Serial.println(" is not a valid character!");
    can = false;
  }
  int tableIndex = findSymbolInTable(character);
  int toX = charTable[tableIndex][0];
  int toY = charTable[tableIndex][1];

  if (toX == -1 || toY == -1) {
    Serial.print("[ERROR] Character "); Serial.print(character); Serial.print(" does not have a valid XY coords!"); Serial.print(" X: "); Serial.print(toX, DEC); Serial.print(" Y: "); + Serial.println(toY, DEC);
    can = false;
  }

  if (tableIndex == -1 || tableIndex == -1) {
    Serial.print("[ERROR] Character "); Serial.print(character); Serial.println(" does not exist in the character list!");
    can = false;
  }

  if(toX > MAX_X || toY > MAX_Y){
    Serial.print("[ERROR] Character "); Serial.print(character); Serial.println(" does not exist in the character list! (Table Overflow)");
    can = false;
  }
  
  if (can) {
    Serial.print("[DEBUG] Moving to character "); Serial.print(character); Serial.print(" ("); Serial.print(character, DEC); Serial.print(")"); Serial.print(" X: "); Serial.print(toX, DEC); Serial.print(" Y: "); + Serial.println(toY, DEC);
    move(toX, toY);
    //line(toX, toY);
  }
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
  Serial.print("[DEBUG - TO] MX: "); Serial.print(moveX, DEC); Serial.print(" dirX: "); Serial.print(dirX, DEC); Serial.print(" TX: "); Serial.print(tx, DEC); Serial.println("");
  Serial.print("[DEBUG - TO] MY: "); Serial.print(moveY, DEC); Serial.print(" dirY: "); Serial.print(dirY, DEC); Serial.print(" TY: "); Serial.print(ty, DEC); Serial.println("");
  if (moveX != 0) {
    motorLR.step(moveX, dirX, DOUBLE);
  }
  if (moveY != 0) {
    motorUD.step(moveY, dirY, DOUBLE);
  }

  update();
  //release();  // remove this if you are using the line function.
  posX = tx;
  posY = ty;

  Serial.print("[DEBUG - POS] X: "); Serial.print(posX, DEC); Serial.print(" Y: "); Serial.print(posY, DEC); Serial.println("");
}

void release()
{
  motorLR.release();
  motorUD.release();
}

// https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
void line(int x1, int y1) {

  int x0 = posX;
  int y0 = posY;
  const int moveAmount = 10;

  int dx = abs(x1 - x0), sx = x0 < x1 ? moveAmount : -moveAmount;
  int dy = abs(y1 - y0), sy = y0 < y1 ? moveAmount : -moveAmount;
  int err = (dx > dy ? dx : -dy) / 2, e2;

  while (true) {
    move(x0, y0);
    update();
    //Serial.print("M("); Serial.print(x0); Serial.print(","); Serial.print(y0); Serial.println(")");
    if (x0 == x1 && y0 == y1)
      break;
    e2 = err;
    if (e2 > -dx) {
      err -= dy;
      x0 += sx;
    }
    if (e2 < dy) {
      err += dx;
      y0 += sy;
    }
  }
}

void spell(char* charArray) {
  for (int i = 0; i <= 32; i++) {
    if (charArray[i] != 0) {
      Serial.print("Character: "); Serial.println(charArray[i]);
      ouijaDelay();
      moveToSymbol(charArray[i]);
      ouijaDelay();
    }
    else break;
  }
}

void delayBetter(int milliSeconds)
{
  long start = millis();
  long end = start + milliSeconds;
  while (millis() < end) {
    delay(1);
    update();
  }
}

void ouijaDelay() {
  delayBetter(1000); //200
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

void letterTest() {
  if (Serial.available() > 0) {
    char in = (char)Serial.read();
    if (in == '~') {
      reset();
    } else {
      moveToSymbol(in);
    }
    release();
  }
}

void serialCalbrate() {
  if (Serial.available() > 0) {
    char in = (char)Serial.read();

    //    if (in == 'w') {
    //      move(posX, posY + 20);
    //    }
    //    else if (in == 'W') {
    //      move(posX, posY + 50);
    //    }
    //
    //    else if (in == 's') {
    //      move(posX, posY - 20);
    //    }
    //    else if (in == 'S') {
    //      move(posX, posY - 50);
    //    }
    //
    //    else if (in == 'a') {
    //      move(posX - 20, posY);
    //    }
    //    else if (in == 'A') {
    //      move(posX - 50, posY);
    //    }
    //
    //    else if (in == 'd') {
    //      move(posX + 20, posY);
    //    }
    //    else if (in == 'D') {
    //      move(posX + 50, posY);
    //    }
    //    else if (in == 'r') {
    //      reset();
    //    }

    if (in == '=') { //w
      move(posX, posY + 30);
    }
    else if (in == ']') { //s
      move(posX, posY - 30);
    }
    else if (in == '[') { //a
      move(posX - 30, posY);
    }
    else if (in == '\\') { //d
      move(posX + 30, posY);
    }
    else if (in == '~') { //reset
      move(1800, 0);
    }

    release();

    Serial.print("[DEBUG - POS] X: "); Serial.print(posX, DEC); Serial.print(" Y: "); Serial.print(posY, DEC); Serial.println("");
  }
}



