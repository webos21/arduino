#include <SoftModem.h>
#include <ctype.h>

SoftModem modem;

const int leftMotorCtrlA = 2;
const int leftMotorCtrlB = 4;
const int leftMotorOnOff = 9;

const int rightMotorCtrlA = 8;
const int rightMotorCtrlB = 12;
const int rightMotorOnOff = 10;

void leftMove(int speed, int bBackward) {
  if (bBackward) {
    digitalWrite(leftMotorCtrlA, HIGH);
    digitalWrite(leftMotorCtrlB, LOW);
  } else {
    digitalWrite(leftMotorCtrlA, LOW);
    digitalWrite(leftMotorCtrlB, HIGH);
  }
  analogWrite(leftMotorOnOff, speed);
}

void rightMove(int speed, int bBackward) {
  if (bBackward) {
    digitalWrite(rightMotorCtrlA, HIGH);
    digitalWrite(rightMotorCtrlB, LOW);
  } else {
    digitalWrite(rightMotorCtrlA, LOW);
    digitalWrite(rightMotorCtrlB, HIGH);
  }
  analogWrite(rightMotorOnOff, speed);
}

void setup()
{
  Serial.begin(57600);

  pinMode(leftMotorCtrlA, OUTPUT);
  pinMode(leftMotorCtrlB, OUTPUT);
  pinMode(leftMotorOnOff, OUTPUT);

  pinMode(rightMotorCtrlA, OUTPUT);
  pinMode(rightMotorCtrlB, OUTPUT);
  pinMode(rightMotorOnOff, OUTPUT);

  digitalWrite (leftMotorOnOff, LOW);
  digitalWrite (rightMotorOnOff, LOW);

  delay(1000);
  modem.begin();
}

void loop() {
  while(modem.available()){
   int c = modem.read();
//  while(Serial.available()) {
//    int c = Serial.read();
    if (isprint(c)) {
      Serial.print("Recv CHR : ");
      Serial.println((char)c);
      // ASDW operation (A:left, S:back, D:right, W:forward, Else:stop)
      if (c == 'a') {
        leftMove(255, 1);
        rightMove(255, 0);
      } else if (c == 's') {
        leftMove(255, 1);
        rightMove(255, 1);
      } else if (c == 'd') {
        leftMove(255, 0);
        rightMove(255, 1);
      } else if (c == 'w') {
        leftMove(255, 0);
        rightMove(255, 0);
      } else {
        leftMove(0, 0);
        rightMove(0, 0);
      }
    } else {
        Serial.print("Recv HEX : ");
        Serial.println(c, HEX);
    }
  }
}


