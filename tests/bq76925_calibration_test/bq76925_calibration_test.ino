/*
  The program find the calibration value and gives them for testing
*/

// include i2c library
#include <Wire.h>

// pin definitions
#define voltage_pin A0

// refernce voltage calibration factors
int8_t vref_gc, vref_oc; // reference voltage gain correction, reference voltage offset correction

// cell voltage correction factors
int8_t vcn_oc[6] = {0}; // cell voltage n offset correction
int8_t vcn_gc[6] = {0}; // cell voltage n gain correction

void setup() {
  // begin serial monitor and i2c
  Serial.begin(9600);
  Wire.begin();

  // set the pin as input 
  pinMode(voltage_pin, INPUT);

  // delay to clear the serial monitor
  delay(2000);
  
  // read all the calibration registers
  /*
   there are total 10 registers
   0x30 to 0x38 and then 0x3B
   read all of them and store them in an array
  */

  // array ti=o store the values
  uint8_t calibration_registers[10] = {0};

  for (uint8_t i=0; i<=8; i++){
    Wire.requestFrom((0x30+i),1);
    while(Wire.available()){
      calibration_registers[i] = Wire.read();
    }
    delay(10);
  }

  Wire.requestFrom((0x3B),1);
  while(Wire.available()){
    calibration_registers[9] = Wire.read();
  }

  // see the read value and compare them against register dump code in repo
  Serial.println("Calibration Register Data");

  for(uint8_t i=0; i<=8; i++){
    Serial.print("0x");
    Serial.print(0x30+i,HEX);
    Serial.print(" : 0b");
    Serial.println(calibration_registers[i],BIN);
  }

  Serial.print("0x3B : 0b");
  Serial.println(calibration_registers[9],BIN);
  
  // calulating the callibration values
  vref_gc = (calibration_registers[0] & 0x0F) | ((calibration_registers[9]<<4)&(0x10));
  vref_oc = (calibration_registers[0]>>4) | ((calibration_registers[9] & 0x06) << 3);

  vcn_gc[0] = (calibration_registers[1] & 0x0F) | ((calibration_registers[7] & 0x40) >> 2);
  vcn_gc[1] = (calibration_registers[2] & 0x0F) | (calibration_registers[7] & 0x10);
  vcn_gc[2] = (calibration_registers[3] & 0x0F) | ((calibration_registers[8] & 0x40) >> 2);
  vcn_gc[3] = (calibration_registers[4] & 0x0F) | (calibration_registers[8] & 0x10);
  vcn_gc[4] = (calibration_registers[5] & 0x0F) | ((calibration_registers[8] & 0x04) << 2);
  vcn_gc[5] = (calibration_registers[6] & 0x0F) | ((calibration_registers[8] & 1) << 4);

  vcn_oc[0] = (calibration_registers[1] >> 4) | ((calibration_registers[7] & 0x80) >> 3);
  vcn_oc[1] = (calibration_registers[2] >> 4) | ((calibration_registers[7] & 0x20) >> 1);
  vcn_oc[2] = (calibration_registers[3] >> 4) | ((calibration_registers[8] & 0x80) >> 3);
  vcn_oc[3] = (calibration_registers[4] >> 4) | ((calibration_registers[8] & 0x20) >> 1);
  vcn_oc[4] = (calibration_registers[5] >> 4) | ((calibration_registers[8] & 0x08) << 1);
  vcn_oc[5] = (calibration_registers[6] >> 4) | ((calibration_registers[8] & 0x02) << 3);

  // Print the calibration vaues
  Serial.println("");
  Serial.println("Calibration Values VREF");
  Serial.print("VREF GC: 0b");
  Serial.println(vref_gc,BIN);
  Serial.print("VREF OC: 0b");
  Serial.println(vref_oc,BIN);

  Serial.println("");
  Serial.println("Calibration Values Cell V Gain Correction");
  char data[13];
  for(uint8_t i=0; i<=5; i++){
    snprintf(data,sizeof(data),"VC[%d] GC: 0b",i);
    Serial.print(data);
    Serial.println(vcn_gc[i],BIN);
  }

  Serial.println("");
  Serial.println("Calibration Values Cell V Offset Correction");
  for(uint8_t i=0; i<=5; i++){
    snprintf(data,sizeof(data),"VC[%d] OC: 0b",i);
    Serial.print(data);
    Serial.println(vcn_oc[i],BIN);
  }

  // interpreting te final correction values as they are signed 2s compliment
  vref_gc = (vref_gc>>4 & 1)?(vref_gc | 0xE0) : vref_gc;
  vref_oc = (vref_oc>>5 & 1)?(vref_oc | 0xC0) : vref_oc;

  for (uint8_t i=0; i<=5; i++)
    vcn_gc[i] = (vcn_gc[i]>>4 & 1)?(vcn_gc[i] | 0xE0) : vcn_gc[i];

  for (uint8_t i=0; i<=5; i++)
    vcn_oc[i] = (vcn_oc[i]>>4 & 1)?(vcn_oc[i] | 0xE0) : vcn_oc[i];

  Serial.println("");
  Serial.println("Calibration Values");
  Serial.print("VREF GC: ");
  Serial.println(vref_gc);
  Serial.print("VREF OC: ");
  Serial.println(vref_oc);
  for(uint8_t i=0; i<=5; i++){
    snprintf(data,sizeof(data),"VC[%d] GC: ",i);
    Serial.print(data);
    Serial.println(vcn_gc[i]);
  }
  for(uint8_t i=0; i<=5; i++){
    snprintf(data,sizeof(data),"VC[%d] OC: ",i);
    Serial.print(data);
    Serial.println(vcn_oc[i]);
  }


}

void loop() {
  // put your main code here, to run repeatedly:

}
