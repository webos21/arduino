#include <SoftModem.h>
#include <ctype.h>

SoftModem modem;

const int ledPin = 8;

void setup()
{
  Serial.begin(57600);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  delay(1000);
  modem.begin();
}

void loop() {
  while(modem.available()){
    int c = modem.read();
    if (isprint(c)) {
      Serial.print("Read CHR(");
      Serial.print((char)c);
      Serial.println(")");
      if (c == 'a') {
        digitalWrite(ledPin, HIGH);
      }
      if (c == 'b') {
        digitalWrite(ledPin, LOW);
      }
    } else {
      Serial.print("Read HEX(");
      Serial.print(c,HEX);
      Serial.println(")");      
    }
  }
  if(Serial.available()){
    modem.write(0xff);
    while(Serial.available()) {
      char c = Serial.read();
      modem.write(c);
//      Serial.print("Write CHR(");
//      Serial.print(c);
//      Serial.println(")");
    }
  }
}


