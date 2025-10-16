#include <Arduino.h>
#include "cJSON.h"
#include "dht22.h"
#define ARBITRARY_ID "955f0437370605b644dd5168f7432ecb91046249"

// Returns monitor data in JSON
char *monitorData(float[6]);

// Constants and state
const int LED = 8;
bool LED_STATE = false;
DHT22 sensor1(0), sensor2(1), sensor3(2);

// Toggles LED state
bool toggleLED() {
  return LED_STATE = !LED_STATE;
}

void setup() {
  Serial.begin(9600);
  Serial.println("Starting...");
  pinMode(LED, OUTPUT);
  delay(1000);
}

void loop() {
  float t1 = sensor1.getTemperature(0), h1 = sensor1.getHumidity();
  float t2 = sensor2.getTemperature(0), h2 = sensor2.getHumidity();
  float t3 = sensor3.getTemperature(0), h3 = sensor3.getHumidity();
  float data[6] = {t1, h1, t2, h2, t3, h3};

  char *result = monitorData(data);

  Serial.print(result);

  digitalWrite(LED, LOW);
  delay(100);
  digitalWrite(LED, HIGH);
  delay(900);
}

char *monitorData(float data[6]) {
  cJSON *result = cJSON_CreateObject();
  cJSON *temp1 = cJSON_CreateNumber(data[0]);
  cJSON *hume1 = cJSON_CreateNumber(data[1]);
  cJSON *temp2 = cJSON_CreateNumber(data[2]);
  cJSON *hume2 = cJSON_CreateNumber(data[3]);
  cJSON *temp3 = cJSON_CreateNumber(data[4]);
  cJSON *hume3 = cJSON_CreateNumber(data[5]);
  cJSON *name = cJSON_CreateString(ARBITRARY_ID);
  cJSON *sensor1 = cJSON_CreateNumber(1);
  cJSON *sensor2 = cJSON_CreateNumber(2);
  cJSON *sensor3 = cJSON_CreateNumber(3);
  cJSON *block1 = cJSON_CreateObject();
  cJSON *block2 = cJSON_CreateObject();
  cJSON *block3 = cJSON_CreateObject();

  cJSON_AddItemToObject(block1, "temperature", temp1);
  cJSON_AddItemToObject(block1, "humidity", hume1);

  cJSON_AddItemToObject(block2, "temperature", temp2);
  cJSON_AddItemToObject(block2, "humidity", hume2);

  cJSON_AddItemToObject(block3, "temperature", temp3);
  cJSON_AddItemToObject(block3, "humidity", hume3);

  cJSON_AddItemToObject(result, "device_id", name);
  cJSON_AddItemToObject(result, "sensor1", block1);
  cJSON_AddItemToObject(result, "sensor2", block2);
  cJSON_AddItemToObject(result, "sensor3", block3);

  char *string = cJSON_Print(result);
  cJSON_Delete(result);
  return string;
}