#include <Wire.h>

void setup() {
  Serial.begin(9600);
  Wire.begin();

  delay(1000);
  Serial.print("Register Dump");

  // Wire.beginTransmission(0x21);
  // Wire.write((1<<4)|2);
  // Wire.endTransmission();
}

void loop() {
  for (int i=0x20; i<0x28; i++){
    Wire.requestFrom(i,1);
    while(Wire.available()){
      uint8_t reg = Wire.read();
      Serial.print(i,HEX);
      Serial.print(" : 0b");
      Serial.println(reg, BIN);
    }
    delay(100);
  }

  for (int i=0x30; i<0x39; i++){
    Wire.requestFrom(i,1);
    while(Wire.available()){
      uint8_t reg = Wire.read();
      Serial.print(i,HEX);
      Serial.print(" : 0b");
      Serial.println(reg, BIN);
    }
    delay(100);
  }

  Wire.requestFrom(0x3B,1);
  while(Wire.available()){
    uint8_t reg = Wire.read();
    Serial.print("0x3B : 0b");
    Serial.println(reg, BIN);
  }
}
