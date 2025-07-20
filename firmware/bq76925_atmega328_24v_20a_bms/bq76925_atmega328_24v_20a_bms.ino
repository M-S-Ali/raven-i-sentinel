#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <Wire.h>
#include "bq76925_driver.h"

LFP_Battery battery;

char json_buffer[35];

SemaphoreHandle_t xBqLibrarySemaphore;

void send_data_over_UART( void *pvParameters );
void read_and_update_cell_voltage( void *pvParameters );
void pack_param( void *pvParameters __attribute__((unused)) );

void setup() {
  Serial.begin(9600);
  Wire.begin();

  battery.init();
  
  if ( xBqLibrarySemaphore == NULL ) {
    xBqLibrarySemaphore = xSemaphoreCreateMutex();
    if ( ( xBqLibrarySemaphore ) != NULL )
      xSemaphoreGive( ( xBqLibrarySemaphore ) );
  }

  xTaskCreate(send_data_over_UART, "send_data_over_UART", 128, NULL, 1, NULL);
  xTaskCreate(read_and_update_cell_voltage, "CellVoltage", 128, NULL, 1, NULL);
}

void loop() {}

void send_data_over_UART( void *pvParameters __attribute__((unused)) ) {  
  for (;;){
    if(xSemaphoreTake(xBqLibrarySemaphore, portMAX_DELAY)){
      snprintf(json_buffer, sizeof(json_buffer),"{\"cv\":[%d,%d,%d,%d,%d,%d]}",
                              battery.cell_v[0],battery.cell_v[1],battery.cell_v[2],
                              battery.cell_v[3],battery.cell_v[4],battery.cell_v[5]);
      Serial.println(json_buffer);

      snprintf(json_buffer, sizeof(json_buffer),"{\"pv\":%d,\"pi\":%d,\"pt\":%d}",
                              battery.pack_v,battery.pack_i,battery.pack_temperature);
      Serial.println(json_buffer);

      snprintf(json_buffer, sizeof(json_buffer),"{\"cb\":[%d,%d,%d,%d,%d,%d]}",
                              battery.cell_bal[0],battery.cell_bal[1],battery.cell_bal[2],
                              battery.cell_bal[3],battery.cell_bal[4],battery.cell_bal[5]);
      Serial.println(json_buffer);
      xSemaphoreGive(xBqLibrarySemaphore);
    }

  vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void read_and_update_cell_voltage( void *pvParameters __attribute__((unused)) ) {
  for (;;){
    if(xSemaphoreTake(xBqLibrarySemaphore, portMAX_DELAY)){
      battery.update_cell_v();
      battery.update_pack_param();
      xSemaphoreGive(xBqLibrarySemaphore);
      vTaskDelay(5);
    }
  }
}
