#include "AFMotor.h"

#define CHAR_TABLE_LEGTH 39

const int LIGHT_PIN = 10;

const int SPEED = 100;
const int X_AMOUNT = 1500;
const int Y_AMOUNT = 500;

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
  Serial.println("Hello World!");
  pinMode(LIGHT_PIN, OUTPUT);
  digitalWrite(LIGHT_PIN, HIGH);
  motorLR.setSpeed(SPEED);
  motorUD.setSpeed(SPEED);
}

void loop() {
  serialCalbrate();
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
  motorLR.step(X_AMOUNT, FORWARD, DOUBLE);
  motorLR.release();
  motorUD.step(Y_AMOUNT, FORWARD, DOUBLE);
  motorUD.release();
  delay(50);
  motorLR.step(X_AMOUNT, BACKWARD, DOUBLE);
  motorLR.release();
  motorUD.step(Y_AMOUNT, BACKWARD, DOUBLE);
  motorUD.release();
  delay(50);
}

void serialCalbrate() {
  if (Serial.available() > 0) {
    int incomingByte = 0;
    incomingByte = Serial.read();

    Serial.print("I received: ");
    Serial.println(incomingByte, DEC);
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
