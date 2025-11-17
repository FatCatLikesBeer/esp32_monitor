// Structs

#ifndef MY_TYPES_H
#define MY_TYPES_H

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

#endif
