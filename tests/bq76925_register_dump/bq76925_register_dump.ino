
/*
* The program is used to test the bq76925
* the program wites a data then dumps all the registeres, so the written value can be verified
* The program also blinks an led when connected to VTH pin used for NTC biasing
* Connect 10k to led to be within safe current limits of the pin. 
*/

// Include I2C Library
#include <Wire.h>

// Prototype of functions
void send_data(uint8_t addr, uint8_t data);

// setup function
void setup() {
  // serial port begin
  Serial.begin(9600);

  // i2c begin
  Wire.begin();

  // give a delay to clear the serial monitor manually
  delay(2000);

  // send the data to be verified
  // send_data(0x25, 5);

  // Dump full register data
  Serial.println("Register Dump");

  // Dump register 0x20 to 0x27
  for (int i=0x20; i<0x28; i++){
    Wire.requestFrom(i,1);
    while(Wire.available()){
      uint8_t reg = Wire.read();
      Serial.print("0x");
      Serial.print(i,HEX);
      Serial.print(" : 0b");
      Serial.println(reg, BIN);
    }
    delay(100);
  }

  // Dump data 0x30 to 0x38
  for (int i=0x30; i<0x39; i++){
    Wire.requestFrom(i,1);
    while(Wire.available()){
      uint8_t reg = Wire.read();
      Serial.print("0x");
      Serial.print(i,HEX);
      Serial.print(" : 0b");
      Serial.println(reg, BIN);
    }
    delay(100);
  }

  // Dump data 0x3B
  Wire.requestFrom(0x3B,1);
  while(Wire.available()){
    uint8_t reg = Wire.read();
    Serial.print("0x3B : 0b");
    Serial.println(reg, BIN);
  }
}

float v;

void loop() {

  // for testing blink an an led connected to VTH pin. The pin is used for NTC biasing
  send_data(0x25, 1<<1);
  delay(1000);
  send_data(0x25, 0);
  delay(1000);
}

// Function to send data
void send_data(uint8_t addr, uint8_t data){
  Wire.beginTransmission(addr);
  Wire.write(data);
  Wire.endTransmission();
  delay(10);
}
