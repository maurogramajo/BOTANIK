#include <Arduino.h>
#include <WiFi.h>
#include <DHT.h>

#define DHTTYPE DHT11
#define LEDPIN  16
#define DHTPIN  22
DHT dht(DHTPIN, DHTTYPE);

static void task_led(void* a) {

	while (1) {

		digitalWrite(LEDPIN, !digitalRead(LEDPIN));

		vTaskDelay(1000 / portTICK_PERIOD_MS);

	}

}

static void task_rcv(void* a) {

  static char dato;
  static float t;
  static char temperatura[7];

	while (1) {
    if(Serial.available() > 0) {
      dato = Serial.read();
      if(dato == 't') {
        t = dht.readTemperature();
        dtostrf(t, 6, 2, temperatura);
        Serial.print("Temperatura: ");
        Serial.print(temperatura);
        Serial.println("°C");
      }
      else if(dato == 'h') {
        t = dht.readHumidity();
        dtostrf(t, 6, 2, temperatura);
        Serial.print("Humedad: ");
        Serial.print(temperatura);
        Serial.println("%");
      }
    }
		 
		vTaskDelay(100 / portTICK_PERIOD_MS);

	}

}

void setup() {
  // put your setup code here, to run once:
  dht.begin();
  pinMode(LEDPIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("Arranqué!");
  Serial.println("Ingresar h para ver la humedad o t para la temperatura...");
  xTaskCreate(task_led, (const char *)"task_ledb", configMINIMAL_STACK_SIZE*2, 0, tskIDLE_PRIORITY+1, 0);
  xTaskCreate(task_rcv, (const char *)"task_rcv", configMINIMAL_STACK_SIZE*2, 0, tskIDLE_PRIORITY+1, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
}