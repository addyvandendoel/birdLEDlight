#include "configuration.h"

#include <ArduinoJson.h>
/*#include <FS.h>  //this needs to be first, or it all crashes and burns...
*/
#include "LittleFS.h"
#include "debugger.h"

#define CONFIG_JSON_BUFFERSIZE 800
#define CONFIG_VERSION "1.0.0 AVD 20230101"

bool shouldSaveConfig;
Config config;
const char* fileNameConfig = "/config.json";

// ____________________________________________________________________________________________________________________
//  Name:   set-ShouldSaveConfig()
//  Usage:  set that we need to save the config
//  Input:  true or false
//  Return:
// ____________________________________________________________________________________________________________________
void setShouldSaveConfig(bool saveConfig) {
	DEBUG_PRINT("Should save config");
	shouldSaveConfig = saveConfig;
}
// ____________________________________________________________________________________________________________________
//  Name:   get-ShouldSaveConfig()
//  Usage:  set that we need to save the config
//  Input:  true or false
//  Return:
// ____________________________________________________________________________________________________________________
bool getShouldSaveConfig() {
	DEBUG_PRINT("get config");
	return shouldSaveConfig;
}

/*___________________________________________________________________________________________________________________________________
Name:   saveConfigCallback
Usage:  set shouldSaveConfig with a callback for a callback function. from wifimanager
Input:  -
Return: -
___________________________________________________________________________________________________________________________________*/
void saveConfigCallback() {
}

// ____________________________________________________________________________________________________________________
//  Name:   saveConfiguration
//  Usage:  save the config in Json to FS
//  Input:  filename to save to, config struct
//  Return: true if succesfull, false if it failed.
// ____________________________________________________________________________________________________________________
void saveConfiguration(const char* filename, const Config& config) {
	DEBUG_PRINT("saving config...");
	if (LittleFS.begin()) {
		File file = LittleFS.open(filename, "w");
		if (!file) {
			DEBUG_PRINT("failed to open config file for writing");
			return;
		}
		StaticJsonDocument<CONFIG_JSON_BUFFERSIZE> doc;
		doc["deviceName"] = config.deviceName;
		doc["mqttServerName"] = config.mqttServerName;
		doc["sw_version"] = config.sw_version;
		// serialize to Json
		if (serializeJson(doc, file) == 0) {
			DEBUG_PRINT("Failed to write to file");
			return;
		}
		// Close the file
		file.close();
	}
}

// ____________________________________________________________________________________________________________________
//  Name:   loadConfiguration (formally readfile())
//  Usage:  load the configuration from a json file
//  Input:  filename, and config to write the loaded json values to
//  Return: none
// ____________________________________________________________________________________________________________________
void loadConfiguration(const char* filename, Config& config) {
	// read configuration from FS json
	DEBUG_PRINT("mounting FS...");
	Serial.println("mounting FS...\r\n");

	// LittleFS.format();
	if (LittleFS.begin()) {
		DEBUG_PRINT("mounted file system");
		if (LittleFS.exists("/config.json")) {
			// file exists, reading and loading
			DEBUG_PRINT("reading config file");
			File configFile = LittleFS.open("/config.json", "r");
			if (configFile) {
				DEBUG_PRINT("opened config file");
				StaticJsonDocument<CONFIG_JSON_BUFFERSIZE> doc;
				DeserializationError Jsonerror = deserializeJson(doc, configFile);
				if (Jsonerror) {
					Serial.println(F("Failed to read file"));
				}
				if (!Jsonerror) {
					if (doc.containsKey("deviceName") &&
						doc.containsKey("deviceNameFriendly") &&
						doc.containsKey("IPlocal") &&
						doc.containsKey("IPgateway") &&
						doc.containsKey("IPsubnet") &&
						doc.containsKey("IPDNSprimary") &&
						doc.containsKey("IPDNSsecondary") &&
						doc.containsKey("ledStatus")) {
						// Copy values from the JsonDocument to the Config
						strlcpy(config.deviceName,                              // <- destination
							doc["deviceName"] | DEFAULT_CONFIG_DEVICENAME,  // <- source
							sizeof(config.deviceName));                     // <- destination's capacity
						DEBUG_PRINT("deviceName read from file:");
						DEBUG_PRINT(config.deviceName);
						// Copy values from the JsonDocument to the Config
						strlcpy(config.deviceNameFriendly,                                       // <- destination
							doc["deviceNameFriendly"] | "NO_DEVICEFRIENDLY_NAME_IN_CONFIG",  // <- source
							sizeof(config.deviceNameFriendly));                              // <- destination's capacity
						DEBUG_PRINT("deviceNameFriendly read from file: ");
						DEBUG_PRINT(config.deviceNameFriendly);
						// Copy values from the JsonDocument to the Config
						strlcpy(config.sw_version,           // <- destination
							CONFIG_VERSION,              // <- source
							sizeof(config.sw_version));  // <- destination's capacity
						DEBUG_PRINT("sw_version read from file: ");
						DEBUG_PRINT(config.sw_version);
						// Copy values from the JsonDocument to the Config
						strlcpy(config.IPlocal, doc["IPlocal"], sizeof(config.IPlocal));
						DEBUG_PRINT("IPlocal read from file: ");
						DEBUG_PRINT(config.IPlocal);
						// Copy values from the JsonDocument to the Config
						strlcpy(config.IPgateway, doc["IPgateway"], sizeof(config.IPgateway));
						DEBUG_PRINT("IPgateway read from file: ");
						DEBUG_PRINT(config.IPgateway);
						// Copy values from the JsonDocument to the Config
						strlcpy(config.IPsubnet, doc["IPsubnet"], sizeof(config.IPsubnet));
						DEBUG_PRINT("IPsubnet read from file: ");
						DEBUG_PRINT(config.IPsubnet);
						// Copy values from the JsonDocument to the Config
						strlcpy(config.IPDNSprimary, doc["IPDNSprimary"], sizeof(config.IPDNSprimary));
						DEBUG_PRINT("IPDNSprimary read from file: ");
						DEBUG_PRINT(config.IPDNSprimary);
						// Copy values from the JsonDocument to the Config
						strlcpy(config.IPDNSsecondary, doc["IPDNSsecondary"], sizeof(config.IPDNSsecondary));
						DEBUG_PRINT("IPDNSsecondary read from file: ");
						DEBUG_PRINT(config.IPDNSsecondary);
						// Copy values from the JsonDocument to the Config
						config.ledStatus = doc["ledStatus"];
						DEBUG_PRINT("ledStatus read from file: ");
						DEBUG_PRINT(config.ledStatus);

						DEBUG_PRINT("\n\nkeys found - all good");
						configFile.close();
						return;
					}
					else {
						DEBUG_PRINT("can't find keys in memory");
						configFile.close();
						return;
					}
				}
			}
			else {
				DEBUG_PRINT("failed to load json config");
				return;
			}
		}
		else {
			DEBUG_PRINT("Config does not exist yet");
			return;
		}
	}
	else {
		DEBUG_PRINT("failed to mount FS");
		return;
	}
	// end read
	return;
}