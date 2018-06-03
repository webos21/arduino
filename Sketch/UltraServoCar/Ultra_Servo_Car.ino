#include <Servo.h>
#include <SoftwareSerial.h>

Servo myServo;
SoftwareSerial BTSerial(7, 8);

const int AIA = 3; /* Motor A */
const int AIB = 5;
const int BIA = 6; /* Motor B */
const int BIB = 11;

const int trigPin = 13; /* Ultrasonic sensor */
const int echoPin = 12; /* Ultrasonic sensor */
int S_pin = 10;         /* Servo motor connected pin */

int F_Dist, L_Dist, R_Dist, L45_Dist, R45_Dist; /* Distance from car */

const int Dist_Min = 27;  /* Minimum distance F, L45, R45 */
const int LR_Dist_Min = 12; /* Minimum distance L, R */

int Max_speed1 = 150; /* forward speed */
int Max_speed2 = 200; /* turn speed */
int Dist; /* measured distance from Ultrasonic sensor */
int repeat = 0; /* checking obstacles 120 times while forwarding */
int compare = 0; /* compare sensors measurement 25 times: error elimination */
const int delay_t = 500; /* car turning delay time */
const int neck_t = 100; /* servo motor neck tunning delay time */

int doFree = 1;

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);
  
  myServo.attach(S_pin);
  myServo.write(80);
  delay(neck_t);
  myServo.write(80);
  delay(neck_t);

  pinMode(AIA, OUTPUT);
  pinMode(AIB, OUTPUT);
  pinMode(BIA, OUTPUT);
  pinMode(BIB, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
}

void loop() {
  if (BTSerial.available()) {
    char value = BTSerial.read();
    Serial.println(value);
    if (value == '1') {
      doFree = 1;
    } else {
      doFree = 0;
      brake();
    }
  }
  if (doFree == 1) {
    forward();
    look_turn();
  
    Dist = U_Sonic(); /* Distance measure */
    BTSerial.print("On Loop Distance = ");
    BTSerial.println(round(Dist));
  
    if (Dist < Dist_Min) {
      compare++;
    }
    if (Dist > Dist_Min) {
      compare = 0;
    }
    if (compare > 25) {
      brake();
      look_brake();
  
      if ((L_Dist > R_Dist) && (L_Dist > F_Dist)) {
        left(delay_t);
      } else if ((R_Dist > L_Dist) && (R_Dist > F_Dist)) {
        right(delay_t);
      } else {
        compare=0;
      }
    }
  }
}

void forward() {
  BTSerial.println("Forward...");
  
  analogWrite(AIA, Max_speed1);
  analogWrite(AIB, 0);
  analogWrite(BIA, Max_speed1);
  analogWrite(BIB, 0);
}

void left(int t) {
  BTSerial.println("Left...");
  
  analogWrite(AIA, 0);
  analogWrite(AIB, Max_speed2);
  analogWrite(BIA, Max_speed2);
  analogWrite(BIB, 0);
  delay(t);
}

void right(int t) {
  BTSerial.println("Right...");
  
  analogWrite(AIA, Max_speed2);
  analogWrite(AIB, 0);
  analogWrite(BIA, 0);
  analogWrite(BIB, Max_speed2);
  delay(t);  
}

void brake() {
  BTSerial.println("Brake...");
  
  analogWrite(AIA, 0);
  analogWrite(AIB, 0);
  analogWrite(BIA, 0);
  analogWrite(BIB, 0);  
}

void look_turn() {
  repeat++;
  if (repeat > 120) {
    BTSerial.println("Look_Turn...");

    look_brake();
    if ((L_Dist < LR_Dist_Min) || (L45_Dist < Dist_Min)) {
      right(delay_t);
    }
    if ((R_Dist < LR_Dist_Min) || (R45_Dist < Dist_Min)) {
      left(delay_t);
    }
    repeat = 0;
  }
}

int U_Sonic() {
  long Dist_cm;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(15);
  digitalWrite(trigPin, LOW);
  Dist_cm = pulseIn(echoPin, HIGH);
  Dist_cm = Dist_cm * 0.017; /* (0.034cm/us) / 2 */
  return round(Dist_cm);
}

void look_brake() {
  BTSerial.println("Look_Brake...");
  
  F_Dist = U_Sonic();
  if (F_Dist < Dist_Min) {
    brake();
  }
  myServo.write(120);
  delay(neck_t);
  L45_Dist = U_Sonic();
  if (L45_Dist < Dist_Min) {
    brake();
  }
  myServo.write(160);
  delay(neck_t);
  L_Dist = U_Sonic();
  if (L_Dist < LR_Dist_Min) {
    brake();
  }
  myServo.write(120);
  delay(neck_t);
  L45_Dist = U_Sonic();
  if (L45_Dist < Dist_Min) {
    brake();
  }
  myServo.write(80); /* front direction */
  delay(neck_t);
  F_Dist = U_Sonic();
  if (F_Dist < Dist_Min) {
    brake();
  }
  myServo.write(40);
  delay(neck_t);
  R45_Dist = U_Sonic();
  if (R45_Dist < Dist_Min) {
    brake();
  }
  myServo.write(0);
  delay(neck_t);
  R_Dist = U_Sonic();
  if (R_Dist < LR_Dist_Min) {
    brake();
  }
  myServo.write(80);
  delay(neck_t);
}

