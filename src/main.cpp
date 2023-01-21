/* BirdLEDLight
*
*
Chip is ESP8266 Mini D1 pro for test
Features: WiFi

Changelog
AVD 20221231    v0.1    Test setup for pwm testing


SET VERSION IN CONFIGURATION.CPP!!!
KEEP THE DEVICENAME IN lowercase !!

TODO still todo:

Config.json layout
{
"deviceName": "birdledlight",
"deviceNameFriendly": "BirdLEDLight",
"IPlocal": "192.168.1.222",  -> change me!! look in networkdocumentation.xlsx
"IPgateway": "192.168.1.1",
"IPsubnet": "255.255.255.0",
"IPDNSprimary": "192.168.1.1",
"IPDNSsecondary": "192.168.1.2",
}

ledStatus options can be found in device.cpp
0  LED is allways off
1  LED is allways on
2  Follow the light (light on, led on etc)
4  Follow the light inverted (light off, led on etc)

*/
#define DEBUGGER
#define EVERY_5_MINUTES 300000  // in milli seconds

#define LEDSTRIPPIN1 12 // FIXME esp port 1 GPIO 12
#define LEDSTRIPPIN2 13 // FIXME	esp port 2 GPIO 13
#define TIMERDETECTPIN 16 // FIXME esp port 3 GPIO 10
#define LEDPIN 14

#define LIGHT_FULL_ON 20
#define LIGHT_FULL_OFF 0
#include <Arduino.h>

#include "OTA.h"
#include "WiFi.h"
#include "configuration.h"
#include "debugger.h"
#include "device.h"
#include "secrets.h"
#include "web.h"


bool wifiStatus = false;
unsigned long interval = EVERY_5_MINUTES;
unsigned long ledFlashInterval = 0;
unsigned long prevtime = 0;
unsigned long prevTimeCheckConnection = 0;
bool ledstate = false;
void flashLED();

// ____________________________________________________________________________________________________________________
//
//  Name:   SETUP
//
// ____________________________________________________________________________________________________________________
void setup() {
	Serial.begin(115200);  // debug
	// config load
	DEBUG_PRINT("Load config\n");
	loadConfiguration(fileNameConfig, config);
	// setup device
	DEBUG_PRINT("SetupDevice\n");
	setupDevice();
	// setup wifi
	DEBUG_PRINT("SetupWiFi\n");
	if ((wifiStatus = WiFiConnection())) {
		// setup OTA
		DEBUG_PRINT("Setup OTA\n");
		setupOTA();
		// setup WebServer
		DEBUG_PRINT("Setup WebServer\n");
		setupWebserver();
	}
	else {
		DEBUG_PRINT("WiFi setup failed! Status:");
		DEBUG_PRINT(wifiStatus);
		ledFlashInterval = 3000;
	}
}
// ____________________________________________________________________________________________________________________
//
//  Name:   Loop
//
// ____________________________________________________________________________________________________________________
void loop() {

	loopDevice(false);

	// check if we have wifi and mqtt, if we do, let's do our trick
	if (wifiStatus) {
		// Listen to OTA to see if we have an firmware update
		checkOTA();
		// check connection to WiFi and MQTT server, as it needs a handshake now and then.
		wifiStatus = checkWiFiconnected();
	}
	else {
		// no wifi or mqtt, no worries, we can still use our good old switches
		flashLED();
		// every 5 minutes we check to see if WiFi and/or MQTT returned.
		unsigned long startTime = millis();
		if (startTime - prevTimeCheckConnection >= interval) {
			prevTimeCheckConnection = startTime;
			// do something 
			wifiStatus = checkWiFiconnected();
			DEBUG_PRINT("Checking WiFi again...\n");
			DEBUG_PRINT(wifiStatus);
			if (wifiStatus) {
				DEBUG_PRINT("WiFi RETURN!! :)\n");
			}
			else {
				DEBUG_PRINT("Still no WiFi :( \n");
			}
		}
	}
}
// ____________________________________________________________________________________________________________________
//  Name:   flashLED
//  Usage:  flash the LED to inform something is wrong
//  Input:  none
//  Return: none
// ____________________________________________________________________________________________________________________

void flashLED() {
	unsigned long startTime = millis();
	if (startTime - prevtime > ledFlashInterval) {
		prevtime = startTime;
		ledstate = !ledstate;
		if (ledstate) {
			digitalWrite(LEDPIN, LOW);
		}
		else {
			digitalWrite(LEDPIN, HIGH);
		}
	}
}
