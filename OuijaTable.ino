#include "AFMotor.h"

const int LIGHT_PIN = 10;
const int LIMIT_LR = A4;
const int LIMIT_UD = A5;

const int SPEED = 100;
const int MIN_X = 400;
const int MIN_Y = 0;
const int MAX_X = 1800;
const int MAX_Y = 700;


const int LIGHT_ON = 4000;
const int LIGHT_OFF = 100;

//planchet's current position
int posX = 0;
int posY = 0;

long timeNextOn = 0;
long timeNextOff = 0;

AF_Stepper motorLR(200, 1);
AF_Stepper motorUD(200, 2);

int charTable[][3] = {
  590, 240, 'a',
  700, 300, 'b',
  840, 340, 'c',
  950, 370, 'd',
  1070, 400, 'e',
  1190, 400, 'f',
  1300, 400, 'g',
  1450, 400, 'h',
  1540, 370, 'i',
  1640, 350, 'j',
  1770, 340, 'k',
  //-1, -1, 'l',
  //-1, -1, 'm',
  //-1, -1, 'n',
  670, 60, 'o',
  790, 120, 'p',
  920, 150, 'q',
  1050, 190, 'r',
  1160, 220, 's',
  1290, 240, 't',
  1400, 220, 'u',
  1530, 200, 'v',
  1650, 140, 'w',
  1800, 110, 'x',
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
  800, 520, '+', //YES
  1800, 520, '-', //NO
  //  -1, -1, '!', //GOODBYE
  1350, 520, '.' //SPACE
};

char* messages[] {
  "seattle.academy", 
  //"hi.human",
  "+",
  "-",
  "i.see.u",
  "are.u.scared",
  "i.am.here.too"
};

#define CHAR_TABLE_LEGTH (sizeof(charTable)/sizeof(charTable[0]))

#define MESSAGES_TABLE_LENGTH (sizeof(messages)/sizeof(messages[0]))

void setup() {
  Serial.begin(9600);
  Serial.println("[INFO] Initalising...");

  pinMode(LIGHT_PIN, OUTPUT);
  digitalWrite(LIGHT_PIN, LOW);

  pinMode(LIMIT_LR, INPUT_PULLUP);
  pinMode(LIMIT_UD, INPUT_PULLUP);

  motorLR.setSpeed(SPEED);
  motorUD.setSpeed(SPEED);

  Serial.println("[INFO] Resetting...");
  reset();
  Serial.print("[INFO] Loaded "); Serial.print(MESSAGES_TABLE_LENGTH); Serial.print(" messages and "); Serial.print(CHAR_TABLE_LEGTH); Serial.println(" letters.");
  Serial.println("[INFO] Successfully Initalised!");
  
  delay(500);
}

//called everytime a message finishes
void loop() {
  update();
  char *msg = messages[random(0,MESSAGES_TABLE_LENGTH)];
      
  spell(msg);
  ouijaDelay();
  spell(".");
  delayBetter(2000);
}

//Called by motor and by loop!
void update() {
  lightFlicker();
}

//Gets called from modified stepper lib
void AF_Stepper::doupdate() {
  update();
}

void reset() {
  // Move until we hit the limit switches
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

void lightFlicker(){
  bool lightIsOn = !digitalRead(LIGHT_PIN);
  int randomChance = LIGHT_ON;
  if(!lightIsOn){
    randomChance = LIGHT_OFF;
  }

  if(random(0, randomChance) == 0){
    digitalWrite(LIGHT_PIN, lightIsOn);
  }
}

int findSymbolInTable(char symbol)
{
  for (int tableIndex = 0; tableIndex < CHAR_TABLE_LEGTH; tableIndex++) {
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

  if (toX == -1 || toY == -1 && can) {
    Serial.print("[ERROR] Character "); Serial.print(character); Serial.print(" does not have a valid XY coords!"); Serial.print(" X: "); Serial.print(toX, DEC); Serial.print(" Y: "); + Serial.println(toY, DEC);
    can = false;
  }

  if (tableIndex == -1 || tableIndex == -1 && can) {
    Serial.print("[ERROR] Character "); Serial.print(character); Serial.println(" does not exist in the character list!");
    can = false;
  }

  if (toX > MAX_X || toY > MAX_Y && can) {
    Serial.print("[ERROR] Character "); Serial.print(character); Serial.println(" does not exist in the character list! (Table Overflow)");
    can = false;
  }

  if (can) {
    //Serial.print("[DEBUG] Moving to character "); Serial.print(character); Serial.print(" ("); Serial.print(character, DEC); Serial.print(")"); Serial.print(" X: "); Serial.print(toX, DEC); Serial.print(" Y: "); + Serial.println(toY, DEC);
    move(toX, toY);
    //line(toX, toY);
  }
}

bool shouldRelease = false;

void move(int tx, int ty) {

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

  if (moveX != 0) {
    motorLR.step(moveX, dirX, DOUBLE);
  }
  if (moveY != 0) {
    motorUD.step(moveY, dirY, DOUBLE);
  }

  update();
  
  posX = tx;
  posY = ty;
}

void release()
{
  motorLR.release();
  motorUD.release();
}

void spell(char* charArray) {
  Serial.print("[INFO] Printing Word: "); Serial.println(charArray);
  for (int i = 0; i <= 32; i++) {
    if (charArray[i] != 0) {
      //Serial.print("Character: "); Serial.println(charArray[i]);
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
  delayBetter(200); //200
}

//===================== Test Code ====================

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
