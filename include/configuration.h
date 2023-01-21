#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include <Arduino.h>

#define MQTT_SERVERNAME_ARRAY_LENGTH 40
#define SENSOR_NAME_ARRAY_LENGTH 30
#define VERSION_ARRAY_LENGTH 30
#define DEVICENAME_ARRAY_LENGTH 20
#define DEVICENAMEFRIENDLY_ARRAY_LENGTH 40
#define IP_ARRAY_LENGTH 16
// 123.567.901.345

// Config Struct
struct Config
{ // full configuration file
    char mqttServerName[MQTT_SERVERNAME_ARRAY_LENGTH];
    char sensorName[SENSOR_NAME_ARRAY_LENGTH];
    char sw_version[VERSION_ARRAY_LENGTH];
    char deviceName[DEVICENAME_ARRAY_LENGTH];
    char deviceNameFriendly[DEVICENAMEFRIENDLY_ARRAY_LENGTH];
    char IPlocal[IP_ARRAY_LENGTH];
    char IPgateway[IP_ARRAY_LENGTH];
    char IPsubnet[IP_ARRAY_LENGTH];
    char IPDNSprimary[IP_ARRAY_LENGTH];
    char IPDNSsecondary[IP_ARRAY_LENGTH];
    int ledStatus;
};

#define DEFAULT_CONFIG_DEVICENAME "NO_DEVICENAME_SET" // Must be unique on the MQTT network
extern const char* fileNameConfig;

extern Config config;
extern int ledBrightness;
extern int fullLightOnTimeSeconds;
void saveConfigCallback();
void saveConfiguration(const char* filename, const Config& config);
void loadConfiguration(const char* filename, Config& config); // old boolean readFile();

#endif