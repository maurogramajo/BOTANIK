#include <Arduino.h>
#include <WiFi.h>

#define LED 16

static void task_led(void* a) {

	while (1) {

		digitalWrite(LED, !digitalRead(LED));

		vTaskDelay(1000 / portTICK_PERIOD_MS);

	}

}

static void task_rcv(void* a) {

	while (1) {
    if(Serial.available() > 0) {
      char dato = Serial.read();
      if(dato == 'b') Serial.println("Sos boton");
      else if(dato == 's') Serial.println("Sos sonso");
    }
		 
		vTaskDelay(100 / portTICK_PERIOD_MS);

	}

}

void setup() {
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);
  Serial.begin(9600);
  Serial.print("Arranqué!\n");
  xTaskCreate(task_led, (const char *)"task_ledb", configMINIMAL_STACK_SIZE*2, 0, tskIDLE_PRIORITY+1, 0);
  xTaskCreate(task_rcv, (const char *)"task_rcv", configMINIMAL_STACK_SIZE*2, 0, tskIDLE_PRIORITY+1, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
}