
#include <Arduino.h>
#include <Wire.h>
#include "bq76925_driver.h"

static void i2c_send_data(uint8_t reg_addr, uint8_t data);

static uint8_t i2c_read_data(uint8_t reg_addr);

void LFP_Battery::init(){
  pinMode(voltage_pin, INPUT);
  pinMode(current_pin, INPUT);
  pinMode(temperature_pin, INPUT);

  i2c_send_data(0x25, 1<<2);

  analogReference(INTERNAL);

  calibrate();

}

void LFP_Battery::update_cell_v(){
  for (uint8_t i=0; i<6; i++){
    i2c_send_data(0x21, (1 << 4) | i);
    for(int j = 0; j<=25000; j++);
    cell_v[i] = (analogRead(voltage_pin)*1.1 / 1023.0 * 100);
  }

}

void LFP_Battery::update_pack_param(){
  int total=0;
  for (uint8_t i=0; i<6; i++)
    total += cell_v[i];
  
  pack_v = total;
  pack_i = analogRead(current_pin);
  pack_temperature = analogRead(temperature_pin);
}

void LFP_Battery::set_balancing(uint8_t cell){
  uint8_t balancing_status; 
  cell_bal[cell-1] = 1;
  balancing_status = i2c_read_data(0x22);
  balancing_status |= (1<<(cell-1));
  i2c_send_data(0x22, balancing_status);
}

void LFP_Battery::reset_balancing(uint8_t cell){
  cell_bal[cell-1] = 0;
  uint8_t balancing_status; 
  balancing_status = i2c_read_data(0x22);
  balancing_status &= ~(1<<(cell-1));
  i2c_send_data(0x22, balancing_status);
}

void LFP_Battery::update_bal_status(){

  uint8_t balacing_status; 
  balacing_status = i2c_read_data(0x22);

  for(uint8_t i=0; i<6; i++)
    cell_bal[i] = (balacing_status>>i) & 1;
}

void LFP_Battery::calibrate(){

  uint8_t calibration_registers[10] = {0};

  for (uint8_t i=0; i<=8; i++)
    calibration_registers[i] = i2c_read_data(0x30 + i);

  calibration_registers[9] = i2c_read_data(0x3B);

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

  vref_gc = (vref_gc>>4 & 1)?(vref_gc | 0xE0) : vref_gc;
  vref_oc = (vref_oc>>5 & 1)?(vref_oc | 0xC0) : vref_oc;

  for (uint8_t i=0; i<=5; i++)
    vcn_gc[i] = (vcn_gc[i]>>4 & 1)?(vcn_gc[i] | 0xE0) : vcn_gc[i];

  for (uint8_t i=0; i<=5; i++)
    vcn_oc[i] = (vcn_oc[i]>>4 & 1)?(vcn_oc[i] | 0xE0) : vcn_oc[i];
}

void i2c_send_data(uint8_t reg_addr, uint8_t data){
  Wire.beginTransmission(reg_addr);
  Wire.write(data);
  Wire.endTransmission();
}

uint8_t i2c_read_data(uint8_t reg_addr){
  uint8_t reg_value;
  Wire.requestFrom((int *)reg_addr, 1);
  while(Wire.available()){
    reg_value = Wire.read();
  }
  return reg_value;
}








