#ifndef WIFI_H
#define WIFI_H
#include <Arduino.h>

boolean WiFiConnection();
boolean checkWiFiconnected();
void getWifiStatus(char *status, size_t statussize);

#endif