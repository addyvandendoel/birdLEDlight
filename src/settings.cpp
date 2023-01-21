#include <ArduinoJson.h>
#include <LittleFS.h>

#include "settings.h"
#include "debugger.h"

Settings settings;
const char* fileNameSettings = "/settings.json";
// Settings, is project specific file with settings. Config is a file with global settings for every project.

int ledBrightness = 0;

// ____________________________________________________________________________________________________________________
//  Name:   saveSettings
//  Usage:  save the settings in Json to FS
//  Input:  filename to save to, settings struct
//  Return: true if succesfull, false if it failed.
// ____________________________________________________________________________________________________________________
void saveSettings(const char* filename, const Settings& settings) {
	DEBUG_PRINT("Save Settings: saving settings...");
	if (LittleFS.begin()) {
		DEBUG_PRINT("Save Settings: mounted file system");
		File settingsFile = LittleFS.open(filename, "w");
		if (!settingsFile) {
			DEBUG_PRINT("Save Settings: failed to open settings file for writing");
			return;
		}
		StaticJsonDocument<SETTINGS_JSON_BUFFERSIZE> doc;
		doc["fullLightOnTimeSeconds"] = settings.fullLightOnTimeSeconds;
		// serialize to Json
		if (serializeJson(doc, settingsFile) == 0) {
			DEBUG_PRINT("Save Settings: Failed to write to file");
			return;
		}
		// Close the file
		settingsFile.close();
	}
}

// ____________________________________________________________________________________________________________________
//  Name:   loadSettings (formally readfile())
//  Usage:  load the settings from a json file
//  Input:  filename, and settings to write the loaded json values to
//  Return: none
// ____________________________________________________________________________________________________________________
void loadSettings(const char* filename, Settings& settings) {
	// read settings from FS json
	DEBUG_PRINT("Load Settings: mounting FS...");
	Serial.println("Load Settings: mounting FS...\r\n");
	if (LittleFS.begin()) {
		DEBUG_PRINT("Load Settings: mounted file system");
		if (LittleFS.exists("/settings.json")) {
			// file exists, reading and loading
			DEBUG_PRINT("Load Settings: reading settings file");
			File settingsFile = LittleFS.open("/settings.json", "r");
			if (settingsFile) {
				DEBUG_PRINT("Load Settings: opened settings file");
				StaticJsonDocument<SETTINGS_JSON_BUFFERSIZE> doc;
				DeserializationError Jsonerror = deserializeJson(doc, settingsFile);
				if (Jsonerror) {
					Serial.println(F("Load Settings: Failed to read file"));
				}
				if (!Jsonerror) {
					if (doc.containsKey("fullLightOnTimeSeconds")) {
						// Copy values from the JsonDocument to the Settings, this is an int, so we can do = instead of strlcpy
						settings.fullLightOnTimeSeconds = doc["fullLightOnTimeSeconds"];
						DEBUG_PRINT("fullLightOnTimeSeconds read from file: ");
						DEBUG_PRINT(settings.fullLightOnTimeSeconds);
						// end settings Copy
						DEBUG_PRINT("\n\nkeys found - all good");
						settingsFile.close();
						return;
					}
					else {
						DEBUG_PRINT("Load Settings: can't find keys in memory");
						settingsFile.close();
						return;
					}
				}
			}
			else {
				DEBUG_PRINT("Load Settings: failed to load json settings");
				return;
			}
		}
		else {
			DEBUG_PRINT("Load Settings: Settings does not exist yet");
			return;
		}
	}
	else {
		DEBUG_PRINT("Load Settings: failed to mount FS");
		return;
	}// end read
	return;
}