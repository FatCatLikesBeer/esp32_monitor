#include "WiFi.h"
#include "cJSON.h"
#include "dht22.h"
#include "esp_mac.h"
#include "jsonmaker.hpp"
#include <Arduino.h>
#include <cmath>
#include <ctime>
#include <string>

#define LED_LOW LOW
#define LED_HIGH HIGH
#define FREE_HEAP_DELTAS 5

// Constants
const int LED_PIN = 9;
const int BUTTON_PIN = 21; // BUTTON NOT IN USE
const char *wifi_ssid = "MySpectrumWiFi00-2G";
const char *wifi_pswd = "proudzebra986";
const char *server = "10.0.0.2";
const int port = 8000;
const String header1 = "POST / HTTP/1.1\n"
                       "Host: ";
const String header2 = "Content-Type: application/json\n"
                       "Content-Length: ";

// State
float t1, t2, t3, h1, h2, h3;
float *sensor_data[6] = {&t1, &h1, &t2, &h2, &t3, &h3};
uint32_t free_heap[2];
int free_heap_deltas[FREE_HEAP_DELTAS],
    delta_array_length = sizeof(free_heap_deltas) / sizeof(free_heap_deltas[0]);

DHT22 sensor1(0), sensor2(1), sensor3(2);
WiFiClient client;
String s_result;
char *json_result;
uint8_t mac[6];
uint64_t chip_id;
String device_id;

/////////////
//// Function Declarations
/////////////
// Blink quickly every 2-3 seconds
void LED_indicate_stable();
// Blink slowly eveery 4-5 seconds
void LED_indicate_warning();

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  digitalWrite(LED_PIN, LED_LOW);

  // Get MAC address
  esp_read_mac(mac, ESP_MAC_WIFI_STA);
  for (int i = 0; i < 6; i++)
    chip_id |= ((uint64_t)mac[i]) << (i * 8);
  device_id = String(chip_id);

  // Init Wifi
  WiFi.begin(wifi_ssid, wifi_pswd);
  byte ssid_total = WiFi.scanNetworks();

  while (WL_CONNECTED != WiFi.status()) {
    Serial.println("Attempting to connect to WIFI");
    Serial.print("Networks found: ");
    Serial.println(ssid_total);

    LED_indicate_warning();
    delay(5000);
  }

  Serial.print("Networks found: ");
  Serial.println(ssid_total);
  Serial.print("WiFi Connected to: ");
  Serial.println(wifi_ssid);

  // Connect to server
  client.connect(server, port);

  delay(1000);
}

void loop() {
  // Get Data
  int connection_retries = 3;
  t1 = sensor1.getTemperature(0), h1 = sensor1.getHumidity();
  t2 = sensor2.getTemperature(0), h2 = sensor2.getHumidity();
  t3 = sensor3.getTemperature(0), h3 = sensor3.getHumidity();

  // Sensor data to cJSON struct
  cJSON *c_result = sensor_data_as_JSON(sensor_data, device_id.c_str());

  // cJSON to c_string
  json_result = cJSON_Print(c_result);
  s_result = String(json_result);

  // Delete cJSON struct
  cJSON_Delete(c_result);
  // Free the damned string!
  free(json_result);

  // Check & retry connection
  while (!client.connected()) {
    LED_indicate_warning();
    client.stop();

    // Retry Loop
    if (connection_retries > 0) {
      client.connect(server, port);
      Serial.printf("Connection Lost: %d more retries.\n\n",
                    connection_retries--);
    }

    // Static Failure
    if (connection_retries <= 0) {
      Serial.println("Could not connect. I've given up :(");
    }

    delay(1000);
  }

  // Print payload to console
  Serial.println("\n--- Begin send ---\n");
  Serial.print(header1);
  Serial.printf("%s:%d\n", server, port);
  Serial.print(header2);
  Serial.printf("%d\n\n", s_result.length());
  Serial.println(s_result);
  Serial.println("--- End send ---\n");

  // Send payload to server
  client.print(header1);
  client.printf("%s:%d\n", server, port);
  client.print(header2);
  client.printf("%d\n\n", s_result.length());
  client.println(s_result);

  // Print response
  Serial.printf("\n\n--- Begin recieve: %d bytes ---\n", client.available());
  while (client.available())
    Serial.printf("%c", client.read());
  Serial.printf("\n\n--- End recieve ---\n");

  // Free Heap delta logic
  free_heap[0] = free_heap[1];
  free_heap[1] = ESP.getFreeHeap();
  for (int i = 0; i < delta_array_length - 1; i++)
    free_heap_deltas[i] = free_heap_deltas[i + 1];
  free_heap_deltas[delta_array_length - 1] = free_heap[1] - free_heap[0];

  // Printf debugging
  Serial.println("--- Debug Info Begin --- ");
  Serial.printf("Free heap current length: %u\n", ESP.getFreeHeap());
  Serial.printf("Free heap previous deltas: %d, %d, %d, %d, %d \n",
                free_heap_deltas[0], free_heap_deltas[1], free_heap_deltas[2],
                free_heap_deltas[3], free_heap_deltas[4]);
  Serial.printf("WiFi status: %d\n", WiFi.status());
  Serial.printf("Client connected: %d\n", client.connected());
  Serial.println("--- Debug Info End --- ");

  // Do Stuff With UI
  LED_indicate_stable();
  delay(10000);
}

// Few quick blinks every 2-3 seconds
void LED_indicate_stable(void) {
  ulong base_time = millis(), on_time = millis();
  bool state = true;

  digitalWrite(LED_PIN, LED_HIGH);
  while (state) {
    if (millis() > (on_time + 200)) {
      on_time = millis();
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }

    if (millis() > (base_time + 1000))
      state = false;
  }
  digitalWrite(LED_PIN, LED_LOW);
}

void LED_indicate_warning(void) {
  ulong base_time = millis(), on_time = millis();
  bool state = true;

  digitalWrite(LED_PIN, LED_HIGH);
  while (state) {
    if (millis() > (on_time + 1000)) {
      on_time = millis();
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    }

    if (millis() > (base_time + 5000))
      state = false;
  }
  digitalWrite(LED_PIN, LED_LOW);
}

// TODO: Figure out why device stops working and shit.
// TODO: Swap arbitrary_id with preprogramm id
