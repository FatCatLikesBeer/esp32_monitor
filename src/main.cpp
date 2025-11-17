#include "WiFi.h"
#include "cJSON.h"
#include "credentials.h"
#include "credentials.secret.h"
#include "dht22.h"
#include "jsonmaker.hpp"
#include <Arduino.h>

#define LED_LOW LOW
#define LED_HIGH HIGH
#define FREE_HEAP_DELTAS 5

// Structs
struct WifiConnection {
  const char *ssid;
  const char *pass;
  const char *server;
  int port;
};

struct SensorData {
  float temperature;
  float humidity;
};

// Constants
const int LED_PIN = 9;
const int DEV_PIN = 4;
const int SENSOR_PIN0 = 0;
const int SENSOR_PIN1 = 1;
const int SENSOR_PIN2 = 2;
const String header1 = "POST / HTTP/1.1\n"
                       "Host: ";
const String header2 = "Content-Type: application/json\n"
                       "Content-Length: ";

// State & Objects
float t1, t2, t3, h1, h2, h3;
float *sensor_data[6] = {&t1, &h1, &t2, &h2, &t3, &h3};
uint32_t free_heap[2];
int free_heap_deltas[FREE_HEAP_DELTAS],
    delta_array_length = sizeof(free_heap_deltas) / sizeof(free_heap_deltas[0]);
DHT22 sensor1(SENSOR_PIN0), sensor2(SENSOR_PIN1), sensor3(SENSOR_PIN2);
WiFiClient client;
String s_result;
char *json_result;
uint8_t mac[6];
uint64_t chip_id;
String device_id;
SensorData *data_container[3];

WifiConnection target_network;

/////////////
//// Function Declarations
/////////////

// Blink quickly every 2-3 seconds
void LED_indicate_stable();
// Blink slowly every 4-5 seconds
void LED_indicate_warning();
// Print Debug Info
void print_debug_info();

void setup() {
  target_network.ssid = DEV_WIFI_SSID;
  target_network.pass = DEV_WIFI_PASS;
  target_network.server = DEV_SERVER_IP;
  target_network.port = DEV_SERVER_PORT;

  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(DEV_PIN, INPUT_PULLDOWN);
  digitalWrite(LED_PIN, LED_LOW);

  // If DEV_PIN is enabled, use dev netwrok
  if (0 == digitalRead(DEV_PIN)) {
    target_network.ssid = WIFI_SSID;
    target_network.pass = WIFI_PASS;
    target_network.server = SERVER_IP;
    target_network.port = SERVER_PORT;
    Serial.println("DEV_PIN: 0");
    Serial.printf("Normal Mode: connecting to %s\n", target_network.ssid);
  } else {
    Serial.println("DEV_PIN: 1");
    Serial.printf("Dev Mode: connecting to %s\n", target_network.ssid);
  }

  // Get MAC address
  esp_read_mac(mac, ESP_MAC_WIFI_STA);
  for (int i = 0; i < 6; i++)
    chip_id |= ((uint64_t)mac[i]) << (i * 8);
  device_id = String(chip_id);

  // Init Wifi
  WiFi.begin(target_network.ssid, target_network.pass);
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
  Serial.println(target_network.ssid);

  // Connect to server
  client.connect(target_network.server, target_network.port);

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
      client.connect(target_network.server, target_network.port);
      Serial.printf("Connection Lost: %d more retries.\n\n",
                    connection_retries--);
    }

    // Static Failure
    if (connection_retries <= 0) {
      print_debug_info();
      Serial.println("Could not connect. I've given up :(");
    }

    print_debug_info();
    delay(1000);
  }

  // Print payload to console
  Serial.println("\n--- Begin send ---\n");
  Serial.print(header1);
  Serial.printf(target_network.server, target_network.port);
  Serial.print(header2);
  Serial.printf("%d\n\n", s_result.length());
  Serial.println(s_result);
  Serial.println("--- End send ---\n");

  // Send payload to server
  client.print(header1);
  client.printf("%s:%d\n", target_network.server, target_network.port);
  client.print(header2);
  client.printf("%d\n\n", s_result.length());
  client.println(s_result);

  // Print response
  Serial.printf("\n\n--- Begin recieve: %d bytes ---\n", client.available());
  while (client.available())
    Serial.printf("%c", client.read());
  Serial.printf("\n\n--- End recieve ---\n");

  print_debug_info();

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

void print_debug_info() {
  // Free Heap delta logic
  free_heap[0] = free_heap[1];
  free_heap[1] = ESP.getFreeHeap();
  for (int i = 0; i < delta_array_length - 1; i++)
    free_heap_deltas[i] = free_heap_deltas[i + 1];
  free_heap_deltas[delta_array_length - 1] = free_heap[1] - free_heap[0];

  // Printf debugging
  Serial.println("--- Debug Info Begin --- ");
  Serial.printf("DEV_PIN: %d\n", digitalRead(DEV_PIN));
  Serial.printf("Free heap current length: %u\n", ESP.getFreeHeap());
  Serial.printf("Free heap previous deltas: %d, %d, %d, %d, %d \n",
                free_heap_deltas[0], free_heap_deltas[1], free_heap_deltas[2],
                free_heap_deltas[3], free_heap_deltas[4]);
  Serial.printf("WiFi status: %d\n", WiFi.status());
  Serial.printf("Client connected: %d\n", client.connected());
  Serial.println("--- Debug Info End --- ");
}
