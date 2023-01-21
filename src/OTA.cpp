#include "OTA.h"

#include <Arduino.h>
#include <ArduinoOTA.h>

#include "secrets.h"

// ____________________________________________________________________________________________________________________
//  Name:   setupOTA
//  Usage:  start the Over The Air
//  Input:  none
//  Return: none
// ____________________________________________________________________________________________________________________
unsigned int oldProgress;
void setupOTA() {  // Start the OTA service

	ArduinoOTA.setHostname(CONFIG_OTA_USER);
	ArduinoOTA.setPassword(CONFIG_OTA_PASS);

	ArduinoOTA.onStart([]() {  //before starting OTA, lets set the defaults for savety
		Serial.println("Start OTA");
	digitalWrite(LED_BUILTIN, LOW);
	String type;
	if (ArduinoOTA.getCommand() == U_FLASH) {
		type = "sketch";

	}
	else {
		// U_SPIFFS
		type = "filesystem";
	}
	// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
	Serial.println("Start updating " + type);
	oldProgress = 0;
		});
	ArduinoOTA.onEnd([]() {
		Serial.println("\r\nEnd");
	digitalWrite(LED_BUILTIN, HIGH);
		});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		unsigned int progressPercentage = (progress / (total / 100));
	if (progressPercentage >= oldProgress + 1) {
		char progressChar[50];
		sprintf(progressChar, "%u%%", progressPercentage);
		Serial.printf("Progress: %u%%\n", progressPercentage);
		//transmitData("OTAprog", progressChar); no longer using bluetooth while OTA is in progress
		digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
		oldProgress = progressPercentage;
	}
		});

	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
	if (error == OTA_AUTH_ERROR)
		Serial.println("Auth Failed");
	else if (error == OTA_BEGIN_ERROR)
		Serial.println("Begin Failed");
	else if (error == OTA_CONNECT_ERROR)
		Serial.println("Connect Failed");
	else if (error == OTA_RECEIVE_ERROR)
		Serial.println("Receive Failed");
	else if (error == OTA_END_ERROR)
		Serial.println("End Failed");
		});
	ArduinoOTA.begin();
	Serial.println("OTA ready\r\n");
}

// ____________________________________________________________________________________________________________________
//  Name:   checkOTA
//  Usage:  check the OTA in the Loop
//  Input:  none
//  Return: none
// ____________________________________________________________________________________________________________________
void checkOTA() {
	ArduinoOTA.handle();
}