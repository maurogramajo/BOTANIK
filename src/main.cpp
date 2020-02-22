#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <DHT.h>
#include <esp_timer.h>

static void timershot(void* arg);

//Defino contador de tiempo oneshot
const esp_timer_create_args_t oneshot_timer_args = {
  .callback = &timershot,
  .arg = 0,
  .dispatch_method = ESP_TIMER_TASK,
  /* name is optional, but may help identify the timer when debugging */
  .name = "oneshot_timer"
};

esp_timer_handle_t oneshot_timer;

bool oneshot_timer_flag = false;


//Creo el objeto dht
#define DHTTYPE DHT11
#define LEDPIN  16
#define DHTPIN  22
DHT dht(DHTPIN, DHTTYPE);

//Coneccion Wifi
const char *ssid = "Telecentro-2fd0     ";
const char *password = "EDZKVDNWCZYW";

//Host
char host[48];
String strhost = "192.168.0.7";
String strurl = "/enviardatos.php";

//Cliente
WiFiClient client;

//Variables globales
String chip_id = "";

//-------Función para Enviar Datos a la Base de Datos SQL--------

void enviardatos(String datos) {

  strhost.toCharArray(host, 49);
  if (!client.connect(host, 80)) {
    Serial.println("Fallo de conexion");
  }

  client.print(String("POST ") + strurl + " HTTP/1.1" + "\r\n" + 
               "Host: " + strhost + "\r\n" +
               "Accept: */*" + "*\r\n" +
               "Content-Length: " + datos.length() + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded" + "\r\n" +
               "\r\n" + datos);           
  //delay(10);             
  
  Serial.print("Enviando datos a SQL...");

  esp_timer_start_once(oneshot_timer, 5000000);
}

static void timershot(void* arg){

  String linea = "No dijo nada xd";
    
  if(client.available() == 0) {
    Serial.println("Cliente fuera de tiempo!");
    client.stop();
  }
  else {
    // Lee todas las lineas que recibe del servidor y las imprime por la terminal serial
    Serial.println("A ver que dice el server...");
    linea = client.readStringUntil('\r');
    Serial.println(linea);
    linea = client.readStringUntil('\r');
    Serial.println(linea);
  }
}

static void tomarmuestra(void* arg) {

  static float t,h;
  static char temperatura[7],humedad[7];

  t = dht.readTemperature();
  dtostrf(t, 6, 2, temperatura);
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println("°C");
  h = dht.readHumidity();
  dtostrf(h, 6, 2, humedad);
  Serial.print("Humedad: ");
  Serial.print(humedad);
  Serial.println("%");
  enviardatos("&chip_id=" + chip_id + "&temperatura=" + temperatura + "&humedad=" + humedad);
}

/*
static void task_led(void* a) {

	while (1) {

		digitalWrite(LEDPIN, !digitalRead(LEDPIN));

		vTaskDelay(1000 / portTICK_PERIOD_MS);

	}

}
*/

/*
static void task_rcv(void* a) {

  static float t,h;
  static char temperatura[7],humedad[7];
  unsigned long currentMillis = millis();

	while (1) {
    if (currentMillis - previousMillis >= 60000) { //envia la temperatura cada 60 segundos
      Serial.println("Entre al if");
      previousMillis = currentMillis;
      t = dht.readTemperature();
      dtostrf(t, 6, 2, temperatura);
      Serial.print("Temperatura: ");
      Serial.print(temperatura);
      Serial.println("°C");
      h = dht.readHumidity();
      dtostrf(h, 6, 2, humedad);
      Serial.print("Humedad: ");
      Serial.print(humedad);
      Serial.println("%");
      enviardatos("chip_id=" + chip_id + "&temperatura=" + temperatura + "&humedad" + humedad);
    }
		 
		vTaskDelay(100 / portTICK_PERIOD_MS);

	}

}
*/

void setup() {
  // put your setup code here, to run once:

  //Inicio Timer periodico
  const esp_timer_create_args_t timer_5min_args = {
    .callback = &tomarmuestra,
    .arg = 0,
    .dispatch_method = ESP_TIMER_TASK,
    /* name is optional, but may help identify the timer when debugging */
    .name = "timer_1min"
  };
  
  esp_timer_handle_t timer_5min;
  esp_timer_create(&timer_5min_args, &timer_5min);

  //Creo el timero oneshot
  esp_timer_create(&oneshot_timer_args, &oneshot_timer);

  //Inicio DHT
  dht.begin();
  
  //Inicio LED
  pinMode(LEDPIN, OUTPUT);

  //Inicio UART
  Serial.begin(9600);
  Serial.println("Arranqué!");
  Serial.print("chipId: ");
  
  unsigned long long1 = (unsigned long)((ESP.getEfuseMac() & 0xFFFF0000) >> 16 );
  unsigned long long2 = (unsigned long)((ESP.getEfuseMac() & 0x0000FFFF));

  chip_id = String(long1, HEX) + String(long2, HEX); // six octets

  Serial.println(chip_id); 

  //Inicio conexión WIFI
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectado");
  Serial.println(WiFi.localIP());

  //Comienza a contar el Timer de 5 minutos
  esp_timer_start_periodic(timer_5min, 300000000);

  //xTaskCreate(task_led, (const char *)"task_led", configMINIMAL_STACK_SIZE*2, 0, tskIDLE_PRIORITY+1, 0);
  //xTaskCreate(task_rcv, (const char *)"task_rcv", configMINIMAL_STACK_SIZE*2, 0, tskIDLE_PRIORITY+1, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
}