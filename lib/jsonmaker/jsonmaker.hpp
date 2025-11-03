#include "cJSON.h"

// Takes data & ID, spits it out as a cJSON object.
char *sensor_data_as_JSON(float data[6], const char device_id[]);
