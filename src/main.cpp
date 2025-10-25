#include "WiFi.h"
#include "cJSON.h"
#include "dht22.h"
#include <Arduino.h>
#include <cmath>
#include <string>

#define ARBITRARY_ID "955f0437370605b644dd5168f7432ecb91046249"
#define LED_LOW LOW
#define LED_HIGH HIGH

// Constants and state
const int LED_PIN = 9;
const int BUTTON_PIN = 21; // BUTTON NOT IN USE
const char *wifi_ssid = "MySpectrumWiFi00-2G";
const char *wifi_pswd = "proudzebra986";
const char *server = "10.0.0.2";
const int port = 8000;
const String header1 = "POST / HTTP/1.1\n"
                       "Host: ";
const String header2 = "Content-Type: application/json\n"
                       "Content-Length:";

DHT22 sensor1(0), sensor2(1), sensor3(2);
WiFiClient client;

/////////////
//// Function Declarations
/////////////
// Pull Data from Sensors
char *sensor_data_as_JSON(float[6]);
// Blink quickly every 2-3 seconds
void LED_indicate_stable();
// Blink slowly eveery 4-5 seconds
void LED_indicate_warning();

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  digitalWrite(LED_PIN, LED_LOW);

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

  delay(1000);
}

void loop() {
  // Get Data
  float t1 = sensor1.getTemperature(), h1 = sensor1.getHumidity();
  float t2 = sensor2.getTemperature(), h2 = sensor2.getHumidity();
  float t3 = sensor3.getTemperature(), h3 = sensor3.getHumidity();
  float data[6] = {t1, h1, t2, h2, t3, h3};

  // Connect to server
  client.connect(server, port);

  // Format Data
  String s_result(sensor_data_as_JSON(data));
  // String s_result(sensor_data_as_JSON(data, arbitrary_id));

  // Send Data
  String header = "POST / HTTP/1.1\n"
                  "Host: 10.0.0.41:8000\n"
                  "Content-Type: application/json\n"
                  "Content-Length:";

  client.print(header);
  client.printf("%d\n\n", s_result.length());
  client.println(s_result);

  // while (client.connected())
  // {
  //   String line = client.readStringUntil('\n');
  //   if (line == "\r") break;
  //   Serial.println(line);
  // }

  // Do Stuff With UI
  client.flush();
  client.stop();
  LED_indicate_stable();
  delay(10000);
}

char *sensor_data_as_JSON(float data[6]) {
  cJSON *result = cJSON_CreateObject();
  cJSON *name = cJSON_CreateString(ARBITRARY_ID);
  cJSON *temp1 = cJSON_CreateNumber(data[0]);
  cJSON *hume1 = cJSON_CreateNumber(data[1]);
  cJSON *temp2 = cJSON_CreateNumber(data[2]);
  cJSON *hume2 = cJSON_CreateNumber(data[3]);
  cJSON *temp3 = cJSON_CreateNumber(data[4]);
  cJSON *hume3 = cJSON_CreateNumber(data[5]);
  cJSON *block1 = cJSON_CreateObject();
  cJSON *block2 = cJSON_CreateObject();
  cJSON *block3 = cJSON_CreateObject();
  cJSON *data_array = cJSON_CreateArray();

  cJSON_AddItemToObject(block1, "temperature", temp1);
  cJSON_AddItemToObject(block1, "humidity", hume1);

  cJSON_AddItemToObject(block2, "temperature", temp2);
  cJSON_AddItemToObject(block2, "humidity", hume2);

  cJSON_AddItemToObject(block3, "temperature", temp3);
  cJSON_AddItemToObject(block3, "humidity", hume3);

  cJSON_AddItemToObject(data_array, "sensor1", block1);
  cJSON_AddItemToObject(data_array, "sensor2", block2);
  cJSON_AddItemToObject(data_array, "sensor3", block3);
  cJSON_AddItemToObject(result, "device_id", name);
  cJSON_AddItemToObject(result, "data", data_array);

  char *string = cJSON_Print(result);
  cJSON_Delete(result);
  return string;
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

// TODO: Fromat doubles to fixed decimal places
// TODO: Reformat header away from `header` to `header1` & `header2`
