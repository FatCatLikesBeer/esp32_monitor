#include "cJSON.h"
#include <lib/my_structs/my_structs.h>

// Takes data & ID, spits it out as a cJSON object.
cJSON *sensor_data_as_JSON(float *data[6], const char device_id[]);
cJSON *sensor_data_as_JSON(SensorData *readings[], const char device_id[]);
