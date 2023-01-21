#ifndef SETTINGS_H
#define SETTINGS_H
#include <Arduino.h>
/*
max length of config.json values. To calculate the JSON object size
{
  "fullLightOnTimeSeconds": 900
}
https://arduinojson.org/v6/assistant/#/step2
result : size to 35  -> rounding to 100, just to be sure.
*/
#define SETTINGS_JSON_BUFFERSIZE 100
extern const char* fileNameSettings;
extern int ledBrightness;


// Settings Struct
struct Settings
{ // full Settingsuration file
  u_int32_t fullLightOnTimeSeconds;
};

extern Settings settings;

void saveSettings(const char* filename, const Settings& Settings);
void loadSettings(const char* filename, Settings& Settings);
#endif