#include "jsonmaker.hpp"
#include "cJSON.h"
#include <lib/my_structs/my_structs.h>
#include <stdio.h>

#define SENSOR_COUNT 3

cJSON *sensor_data_as_JSON(float *data[6], const char device_id[]) {
  char name_buffer[32];
  cJSON *p_names[SENSOR_COUNT];
  cJSON *result = cJSON_CreateObject();
  cJSON *name = cJSON_CreateString(device_id);
  cJSON *data_array = cJSON_CreateArray();

  for (int i = 0; i < SENSOR_COUNT; i++) {
    sprintf(name_buffer, "%s%s%d", device_id, "_", i + 1);

    cJSON *object = cJSON_CreateObject();
    cJSON *name = cJSON_CreateString(name_buffer);
    cJSON *temperature = cJSON_CreateNumber(*data[i]);
    cJSON *humidity = cJSON_CreateNumber(*data[i + 1]);

    cJSON_AddItemToObject(object, "name", name);
    cJSON_AddItemToObject(object, "temperature", temperature);
    cJSON_AddItemToObject(object, "humidity", humidity);

    cJSON_AddItemToArray(data_array, object);
  }

  cJSON_AddItemToObject(result, "device_id", name);
  cJSON_AddItemToObject(result, "data", data_array);

  return result;
}

cJSON *sensor_data_as_JSON(SensorData *readings[], const char device_id[]) {
  char name_buffer[32];
  cJSON *p_names[SENSOR_COUNT];
  cJSON *result = cJSON_CreateObject();
  cJSON *name = cJSON_CreateString(device_id);
  cJSON *data_array = cJSON_CreateArray();

  for (int i = 0; i < SENSOR_COUNT; i++) {
    sprintf(name_buffer, "%s%s%d", device_id, "_", i + 1);

    cJSON *object = cJSON_CreateObject();
    cJSON *name = cJSON_CreateString(name_buffer);
    cJSON *temperature = cJSON_CreateNumber(readings[i]->temperature);
    cJSON *humidity = cJSON_CreateNumber(readings[i]->humidity);

    cJSON_AddItemToObject(object, "name", name);
    cJSON_AddItemToObject(object, "temperature", temperature);
    cJSON_AddItemToObject(object, "humidity", humidity);

    cJSON_AddItemToArray(data_array, object);
  }

  cJSON_AddItemToObject(result, "device_id", name);
  cJSON_AddItemToObject(result, "data", data_array);

  return result;
}
