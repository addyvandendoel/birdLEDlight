#ifndef DEVICE_H
#define DEVICE_H

#define DEVICE_NAME_ARRAY_LENGTH 30
#include <Arduino.h>
struct Device
{
  bool state;
  char name[DEVICE_NAME_ARRAY_LENGTH];
  bool switchVirtual;
  bool switchVirtualSet;
};

// Would be nice to have this less global, but hey, this is my project and I decide what i but my energy in.
extern Device device;

void setupDevice();
void getLEDStatus(char* status, size_t statussize);
void loopDevice(bool wifiStatus);
void setFullOnTime(void);
#endif