#define voltage_pin A0

#include <Wire.h>

struct battery_info {
  float cell_V[6];
  float pack_V;
  float pack_I;
  float pack_temperature;
  byte cell_balancing_status[6];
};

char json_buffer[128];

battery_info battery_data = {0};

void update_json(){
  snprintf(json_buffer, sizeof(json_buffer),
                "{\"v\":[%.2f,%.2f,%.2f,%.2f,%.2f,%.2f],"
                "\"pv\":%.2f,\"pi\":%.2f,\"pt\":%.2f,"
                "\"cb\":[%d,%d,%d,%d,%d,%d]}",
                battery_data.cell_V[0], battery_data.cell_V[1], battery_data.cell_V[2],
                battery_data.cell_V[3], battery_data.cell_V[4], battery_data.cell_V[5],
                battery_data.pack_V, battery_data.pack_I, battery_data.pack_temperature,
                battery_data.cell_balancing_status[0], battery_data.cell_balancing_status[1],
                battery_data.cell_balancing_status[2], battery_data.cell_balancing_status[3],
                battery_data.cell_balancing_status[4], battery_data.cell_balancing_status[5]);
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  
  Serial.println(json_buffer);

}

void loop() {
  for (byte i = 0; i < 8; i++) {
        Wire.beginTransmission(0x21);
        Wire.write((1 << 4) | i);
        Wire.endTransmission();

        Wire.requestFrom(0x21, 1);    // request 6 bytes from slave device #8

        while (Wire.available()) { // slave may send less than requested
        byte c = Wire.read(); // receive a byte as character
        Serial.println(c);         // print the character
        }
        delay(100);
        Serial.println(analogRead(voltage_pin) * (5.0 / 1023.0));
        delay(2000);
  }
  // update_json();
  // // Serial.println(json_buffer);
  // Serial.println(battery_data.cell_V[0]);
  // delay(1000);

}
