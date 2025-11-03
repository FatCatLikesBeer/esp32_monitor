#include "jsonmaker.hpp"
#include "cJSON.h"

char *sensor_data_as_JSON(float data[6], const char device_id[]) {
  cJSON *result = cJSON_CreateObject();
  cJSON *name = cJSON_CreateString(device_id);
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
