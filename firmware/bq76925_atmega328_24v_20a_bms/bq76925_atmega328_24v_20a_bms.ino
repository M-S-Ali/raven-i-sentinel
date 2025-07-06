#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <Wire.h>

#define voltage_pin A0
#define current_pin A1
#define temperature_pin A2

struct battery_info {
  int cell_V[6];
  int pack_V;
  int pack_I;
  int pack_temperature;
  byte cell_bal[6];
};

battery_info battery_data = {0};

char json_buffer[35];

SemaphoreHandle_t xSerialSemaphore;
SemaphoreHandle_t xI2CSemaphore;
SemaphoreHandle_t xBatteryDataSemaphore;

void send_data_over_UART( void *pvParameters );
void read_and_update_cell_voltage( void *pvParameters );
void pack_param( void *pvParameters __attribute__((unused)) );

void setup() {
  Serial.begin(9600);
  Wire.begin();

  pinMode(voltage_pin, INPUT);
  pinMode(current_pin, INPUT);
  pinMode(temperature_pin, INPUT);

  
  if ( xSerialSemaphore == NULL ) {
    xSerialSemaphore = xSemaphoreCreateMutex();
    if ( ( xSerialSemaphore ) != NULL )
      xSemaphoreGive( ( xSerialSemaphore ) );
  }

  if ( xI2CSemaphore == NULL ) {
  xI2CSemaphore = xSemaphoreCreateMutex();
  if ( ( xI2CSemaphore ) != NULL )
    xSemaphoreGive( ( xI2CSemaphore ) );
  }

  if ( xBatteryDataSemaphore == NULL ) {
    xBatteryDataSemaphore = xSemaphoreCreateMutex();
    if ( ( xBatteryDataSemaphore ) != NULL )
      xSemaphoreGive( ( xBatteryDataSemaphore ) );
  }

  xTaskCreate(send_data_over_UART, "send_data_over_UART", 128, NULL, 1, NULL);
  xTaskCreate(read_and_update_cell_voltage, "CellVoltage", 128, NULL, 1, NULL);
  xTaskCreate(pack_param, "PackParam", 128, NULL, 1, NULL);
}

void loop() {}

void send_data_over_UART( void *pvParameters __attribute__((unused)) ) {  
  for (;;){
    
    if ( xSemaphoreTake( xSerialSemaphore, portMAX_DELAY ) ){
      snprintf(json_buffer, sizeof(json_buffer),"{\"cv\":[%d,%d,%d,%d,%d,%d]}",
                              battery_data.cell_V[0],battery_data.cell_V[1],battery_data.cell_V[2],
                              battery_data.cell_V[3],battery_data.cell_V[4],battery_data.cell_V[5]);
      Serial.println(json_buffer);

      snprintf(json_buffer, sizeof(json_buffer),"{\"pv\":%d,\"pi\":%d,\"pt\":%d}",
                              battery_data.pack_V,battery_data.pack_I,battery_data.pack_temperature);
      Serial.println(json_buffer);

      snprintf(json_buffer, sizeof(json_buffer),"{\"cb\":[%d,%d,%d,%d,%d,%d]}",
                              battery_data.cell_bal[0],battery_data.cell_bal[1],battery_data.cell_bal[2],
                              battery_data.cell_bal[3],battery_data.cell_bal[4],battery_data.cell_bal[5]);
      Serial.println(json_buffer);      
      
      xSemaphoreGive( xSerialSemaphore ); 
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void read_and_update_cell_voltage( void *pvParameters __attribute__((unused)) ) {

  for (;;){
    for (byte i = 0; i<6; i++){

      if(xSemaphoreTake(xI2CSemaphore, portMAX_DELAY)){
        Wire.beginTransmission(0x21);
        Wire.write((1 << 4) | i);
        Wire.endTransmission();
        xSemaphoreGive(xI2CSemaphore);
      }

      if ( xSemaphoreTake( xBatteryDataSemaphore, portMAX_DELAY )){
        battery_data.cell_V[i] = (analogRead(voltage_pin)* (5.0 / 1023.0)*100);
        xSemaphoreGive( xBatteryDataSemaphore ); 
      }

      // if ( xSemaphoreTake( xSerialSemaphore, portMAX_DELAY )){
      //   Serial.print("I2C Data updated: ");
      //   Serial.println(i);
      //   xSemaphoreGive( xSerialSemaphore ); 
      // }
    }
    vTaskDelay(5);
    // vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void pack_param( void *pvParameters __attribute__((unused)) ){
  for(;;){
    int total = 0;

    if ( xSemaphoreTake( xBatteryDataSemaphore, portMAX_DELAY )){
      for (byte i=0; i<6; i++){
        total += battery_data.cell_V[i];
      }
      battery_data.pack_V = total;
      battery_data.pack_I = analogRead(current_pin);
      battery_data.pack_temperature = analogRead(temperature_pin);
      xSemaphoreGive( xBatteryDataSemaphore ); 
    }

    // if ( xSemaphoreTake( xSerialSemaphore, portMAX_DELAY )){
    //   Serial.println("Pack Voltage Updated");
    //   xSemaphoreGive( xSerialSemaphore ); 
    // }
    // vTaskDelay(pdMS_TO_TICKS(1000));
    vTaskDelay(5);   
  }
}
